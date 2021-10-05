#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>
#include "Reflection.h"
#include <fstream>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

namespace Ry
{
	static std::string CanonArrayList = "class Ry::ArrayList";
	
	enum RecordType
	{
		Class,
		Struct,
		Union
	};
	
	struct ReflectedRecord
	{
		const CXXRecordDecl* Decl;
		RecordType Type;
		std::string Name;
		
		std::vector<const FieldDecl*> Fields;
		std::vector<const FunctionDecl*> Functions;
	};

	class ReflectionCodeGenerator : public MatchFinder::MatchCallback
	{
	public:

		void LogErr(FullSourceLoc Location, std::string Err)
		{
			std::string SourceLoc = "[line " + std::to_string(Location.getLineNumber()) + ", column " + std::to_string(Location.getColumnNumber()) + "]";
			ErrorMsg = SourceLoc + ": " + Err + "\n";
		}

		bool IsValidReflectionClass(const CXXRecordDecl* Record)
		{
			// Check class doesn't use multiple inheritance
			int BaseClassCount = Record->getNumBases();
			if (BaseClassCount > 1)
				return false;

			// Check class has a parent
			if (BaseClassCount == 0)
				return false;

			// Check class is a child of Ry::Object
			if (BaseClassCount == 1)
			{
				std::string BaseName = Record->bases_begin()->getType().getCanonicalType().getAsString();
				if (BaseName != "class Ry::Object")
					return false;
			}

			return true;
		}

		void CheckClassWithLog(FullSourceLoc Loc, const CXXRecordDecl* Record)
		{			
			// Check class doesn't use multiple inheritance
			int BaseClassCount = Record->getNumBases();
			if (BaseClassCount > 1)
			{
				ErrorMsg = "Multiple inheritance for reflected objects not allowed";
			}

			// Check class has a parent
			if (BaseClassCount == 0)
			{
				ErrorMsg = "Reflected object must inherit from Ry::Object";
			}

			// Check class is a child of Ry::Object
			if(BaseClassCount == 1)
			{
				std::string BaseName = Record->bases_begin()->getType().getCanonicalType().getAsString();
				if (BaseName != "class Ry::Object")
				{
					ErrorMsg = "Reflected object must inherit from Ry::Object";
				}
			}

			if(!ErrorMsg.empty())
			{
				LogErr(Loc, ErrorMsg);
			}
			
		}

		bool IsDeclReflected(const Decl* Declaration)
		{
			// Look for reflection annotation
			if (Declaration->hasAttrs())
			{
				Decl::attr_iterator AttribItr = Declaration->attr_begin();

				while (AttribItr != Declaration->attr_end())
				{
					Attr* Attrib = *AttribItr;

					if (Attrib->getKind() == attr::Annotate)
					{
						AnnotateAttr* AnnotationAttrib = static_cast<AnnotateAttr*>(Attrib);

						if (AnnotationAttrib->getAnnotation().str() == "Reflect")
						{
							return true;
						}

					}

					++AttribItr;
				}
			}

			return false;
		}

		bool IsArrayList(QualType Type)
		{
			return Type.getCanonicalType().getAsString().find(CanonArrayList) == 0;
		}

		bool IsValidContainerType(QualType Type)
		{
			if (Type->isIntegerType() || Type->isFloatingType())
				return true;

			if (const CXXRecordDecl* Record = Type->getAsCXXRecordDecl())
				if (IsValidReflectionClass(Record))
					return true;
			
			return false;
		}
		
		bool IsValidReflectionType(const clang::ASTContext* Context, const FieldDecl* InField = nullptr)
		{
			QualType Type = InField->getType();
			if (Type->isIntegerType())
				return true;

			if (Type->isFloatingType())
				return true;

			if(Type->isStructureOrClassType())
			{
				// todo: support reflected strings
				if(IsArrayList(Type))
				{

					// Create an AST visitor for the template parameters
					struct SpecificationArgumentVisitor : public clang::RecursiveASTVisitor<SpecificationArgumentVisitor>
					{
						bool bError = false;
						const SourceManager& SM;
						ReflectionCodeGenerator& CodeGen;

						SpecificationArgumentVisitor(const SourceManager& Man, ReflectionCodeGenerator& Gen): SM(Man), CodeGen(Gen){}

						bool VisitTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc Specialization)
						{
							if(Specialization.getNumArgs() != 1)
							{
								bError = true;
							}
							else
							{
								TemplateArgumentLoc ArgLoc = Specialization.getArgLoc(0);
								bError = !CodeGen.IsValidContainerType(ArgLoc.getTypeSourceInfo()->getType());
							}
							
							return true;
						}
					};


					// Traverse the template parameters
					TypeLoc Loc = InField->getTypeSourceInfo()->getTypeLoc();
					SpecificationArgumentVisitor ArgumentVisitor(Context->getSourceManager(), *this);					
					ArgumentVisitor.TraverseDecl(const_cast<FieldDecl*>(InField));

					return !ArgumentVisitor.bError;
				}
			}

			if(const CXXRecordDecl* PointerTo = Type->getPointeeCXXRecordDecl())
			{
				if (IsValidReflectionClass(PointerTo))
					return true;
			}

			return false;
		}

		void GenerateReflectedRecord(const ReflectedRecord& Record)
		{

			std::string QualifiedName = Record.Decl->getQualifiedNameAsString();

			// Create generated body that should be manually placed inside of the reflected class
			GeneratedSource << "#undef GeneratedBody" << std::endl; // Undefined it here in case it was already defined
			GeneratedSource << "#define GeneratedBody() \\" << std::endl;
			GeneratedSource << "static const Ry::ReflectedClass* \\" << std::endl;
			GeneratedSource << "GetStaticClass() \\" << std::endl;
			GeneratedSource << "{ \\" << std::endl;
			{
				// Declare class
				GeneratedSource << "\tstatic Ry::ReflectedClass C; \\" << std::endl;

				// Reflected class name
				GeneratedSource << "\tC.Name = \"" << Record.Name << "\";\\" << std::endl;

				// Resize fields member
				GeneratedSource << "\tC.Fields.SetSize(" << Record.Fields.size() << ");\\" << std::endl;

				// Reflect all fields
				for (int FieldIndex = 0; FieldIndex < Record.Fields.size(); FieldIndex++)
				{
					const FieldDecl* Decl = Record.Fields[FieldIndex];

					std::string FieldType = Decl->getType().getAsString();

					std::string FieldsVar = "C.Fields[" + std::to_string(FieldIndex) + "]";

					GeneratedSource << "\t" << FieldsVar << ".Name = \"" << Decl->getNameAsString() << "\";\\" << std::endl;
					GeneratedSource << "\t" << FieldsVar << ".Offset = offsetof(" << QualifiedName << ", " << Decl->getNameAsString() << ");\\" << std::endl;
					
					if(Decl->getType()->isIntegerType() || Decl->getType()->isFloatingType() || IsArrayList(Decl->getType()))
					{
						GeneratedSource << "\t" << FieldsVar << ".Type = GetType<" << FieldType << ">();\\" << std::endl;
						GeneratedSource << "\t" << FieldsVar << ".ObjectClass = nullptr;\\" << std::endl;
					}
					else if(Decl->getType()->isPointerType())
					{
						if(const CXXRecordDecl* PointerTo = Decl->getType()->getPointeeCXXRecordDecl())
						{
							// Pointer type was already determined to have a valid pointer-to
							std::string DeclQualifiedType = PointerTo->getQualifiedNameAsString();
							GeneratedSource << "\t" << FieldsVar << ".Type = " << DeclQualifiedType << "::GetStaticType();\\" << std::endl;
							GeneratedSource << "\t" << FieldsVar << ".ObjectClass = " << DeclQualifiedType << "::GetStaticClass();\\" << std::endl;
						}
					}
					

				}

				// Reflect all functions/methods
				for (int FunctionIndex = 0; FunctionIndex < Record.Functions.size(); FunctionIndex++)
				{
					const FunctionDecl* Decl = Record.Functions[FunctionIndex];
				}

				GeneratedSource << "\treturn &C;\\" << std::endl;
			}
			GeneratedSource << "}\\" << std::endl;

			GeneratedSource << "virtual const Ry::ReflectedClass* \\" << std::endl;
			GeneratedSource << "GetClass() \\" << std::endl;
			GeneratedSource << "{ \\" << std::endl;
			{
				GeneratedSource << "\treturn " << QualifiedName << "::GetStaticClass();\\" << std::endl;
			}
			GeneratedSource << "}\\" << std::endl;

			// Generate GetTypeImpl for reflected class
			// This is used so all fields of a class can be stored in an array of types
			GeneratedSource << "static const Ry::DataType*\\" << std::endl;
			GeneratedSource << "GetStaticType()\\" << std::endl;
			GeneratedSource << "{\\" << std::endl;
			{
				// Declare class
				GeneratedSource << "\tstatic Ry::DataType Result;\\" << std::endl;
				GeneratedSource << "\tResult.Size = sizeof(" <<  QualifiedName << ");\\" << std::endl;
				GeneratedSource << "\tResult.Name = \"" << QualifiedName << "\";\\" << std::endl;
				GeneratedSource << "\tResult.Class = Ry::TypeClass::Object;\\" << std::endl;
				GeneratedSource << "\treturn &Result;\\" << std::endl;
			}
			GeneratedSource << "}" << std::endl << std::endl;

			// Generate template specialization of GetClassImpl
			// This is needed so the class of a certain object can be retrieved even without an instance to that object
			// However a convenience function GetClass() is generated in the Object itself
			// GeneratedSource << "template <>" << std::endl;
			// GeneratedSource << "inline const Ry::ReflectedClass*" << std::endl;
			// GeneratedSource << "GetClassImpl<" << QualifiedName << ">(Ry::TypeTag<" << QualifiedName << ">) " << std::endl;
			// GeneratedSource << "{" << std::endl;
			// {
			//
			//
			// }
			// GeneratedSource << "}" << std::endl << std::endl;
		}
		
		virtual void run(const MatchFinder::MatchResult& Result)
		{
			const Decl* Declaration = Result.Nodes.getNodeAs<clang::Decl>("id");
			const TranslationUnitDecl* TUnitDecl = Declaration->getTranslationUnitDecl();

			if (!IsDeclReflected(Declaration))
				return;
			
			bool bIsRecord = Declaration->getKind() == Decl::CXXRecord;
			bool bIsMethod = Declaration->getKind() == Decl::CXXMethod;
			bool bIsField = Declaration->getKind() == Decl::Field;

			FullSourceLoc Loc = Result.Context->getFullLoc(Declaration->getLocation());

			if (bIsRecord)
			{
				const CXXRecordDecl* AsRecord = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");

				// Ensure proper inheritance
				CheckClassWithLog(Loc, AsRecord);
				if (ProducedError())
					return;

				// Start new record
				ReflectedRecord NewRecord;
				NewRecord.Name = AsRecord->getNameAsString();
				NewRecord.Decl = AsRecord;

				ReflectedRecords.push_back(NewRecord);
				RecordDecls.insert(AsRecord);
			}
			else
			{
				if (!bIsMethod && !bIsField)
				{
					LogErr(Loc, std::string("Unexpected reflection declaration ") + Declaration->getDeclKindName());
					return;
				}

				ReflectedRecord& CurRecord = ReflectedRecords[ReflectedRecords.size() - 1];

				if (bIsField)
				{
					const FieldDecl* AsField = Result.Nodes.getNodeAs<clang::FieldDecl>("id");

					// Check that this field's record has been reflected properly
					const RecordDecl* ContainingRecord = AsField->getParent();
					if (RecordDecls.find(ContainingRecord) == RecordDecls.end())
						return;

					std::string FieldType = AsField->getType().getCanonicalType().getAsString();

					// Check valid type names
					if (IsValidReflectionType(Result.Context, AsField))
					{
						CurRecord.Fields.push_back(AsField);
					}
					else
					{
						LogErr(Loc, "Not a valid type to reflect: " + FieldType);
					}

				}
				else if (bIsMethod)
				{
					const CXXMethodDecl* AsMethod = Result.Nodes.getNodeAs<clang::CXXMethodDecl>("id");

					// Check that this functions's record has been reflected properly
					const RecordDecl* ContainingRecord = AsMethod->getParent();
					if (RecordDecls.find(ContainingRecord) == RecordDecls.end())
						return;

					CurRecord.Functions.push_back(AsMethod);
				}

			}

			
		}

		virtual void onStartOfTranslationUnit()
		{
			
		}

		virtual void onEndOfTranslationUnit()
		{
			// Generate reflection source
			GeneratedSource << "#include \"Core/Reflection.h\"" << std::endl;
			
			for(const ReflectedRecord& Rec : ReflectedRecords)
			{
				GenerateReflectedRecord(Rec);
			}
			
		}

		std::vector<ReflectedRecord>& GetReflectedRecords()
		{
			return ReflectedRecords;
		}

		std::string GetError()
		{
			return ErrorMsg;
		}

		std::string GetErrorMsg()
		{
			return ErrorMsg;
		}

		bool ProducedError()
		{
			return !ErrorMsg.empty();
		}

		std::string GetGeneratedCode()
		{
			return GeneratedSource.str();
		}
		
	private:

		std::string ErrorMsg;

		std::vector<ReflectedRecord> ReflectedRecords;
		std::set<const RecordDecl*> RecordDecls;

		std::ostringstream GeneratedSource;

	};

	static llvm::cl::OptionCategory MyToolCategory("my-tool options");

	bool GenerateReflectionCode(std::string ModuleNameCaps, 
		std::string SourcePath, 
		std::string Include, 
		std::vector<std::string> Includes, 
		std::string& GeneratedSource, 
		std::string& ErrorMsg,
		int Thread)
	{

		MatchFinder Finder;
		ReflectionCodeGenerator CodeGenerator;

		DeclarationMatcher ClassMatcher
			= cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate), isExpansionInFileMatching(Include));

		DeclarationMatcher PropertyMatcher
			= fieldDecl(decl().bind("id"), hasAttr(attr::Annotate), isExpansionInFileMatching(Include));

		DeclarationMatcher FunctionMatcher
			= cxxMethodDecl(decl().bind("id"), hasAttr(attr::Annotate), isExpansionInFileMatching(Include));

		Finder.addMatcher(ClassMatcher, &CodeGenerator);
		Finder.addMatcher(PropertyMatcher, &CodeGenerator);
		Finder.addMatcher(FunctionMatcher, &CodeGenerator);

		std::vector<std::string> Args;

		//Args.push_back("clang++");

		Args.push_back("MyTool");

		Args.push_back(SourcePath);

		Args.push_back("--");

		// Add include paths
		for (std::string& IncludePath : Includes)
		{
			Args.push_back("--include-directory");
			Args.push_back(IncludePath);
		}

		// Tell clang our module name
		Args.push_back("-D");
		Args.push_back("COMPILE_MODULE_" + ModuleNameCaps);

		Args.push_back("-xc++");

		Args.push_back("-std=c++17");

		//Args.push_back("--help");

		//Args.push_back("clang++");

		//Args.push_back("clang");


		//Args.push_back("-c");

		//Args.push_back("--help");
		
		int ArgC = Args.size();
		char** ClangOptions = new char* [Args.size()];
		for(uint32_t Arg = 0; Arg < Args.size(); Arg++)
		{
			ClangOptions[Arg] = new char[Args[Arg].size() + 1];
			strcpy_s(ClangOptions[Arg], Args[Arg].size() + 1, Args[Arg].c_str());			
		}

		std::vector<std::string> Sources = { SourcePath };

		// for (std::string Arg : Args)
		// 	std::cout << Arg << std::endl;
		// std::cout << std::endl;

		clang::tooling::CommonOptionsParser* Parser = new CommonOptionsParser(ArgC, const_cast<const char**>(ClangOptions), MyToolCategory);
		clang::tooling::ClangTool* Tool = new ClangTool(Parser->getCompilations(), Sources);
		Tool->setRestoreWorkingDir(false);
		//Tool.appendArgumentsAdjuster(Parser.getArgumentsAdjuster());
		
		std::unique_ptr<FrontendActionFactory> Fac = newFrontendActionFactory(&Finder);
		int Result = Tool->run(Fac.get());

		//int Result = runToolOnCodeWithArgs(Fac->create(), SourcePath, Args);

		//std::unique_ptr<ASTUnit> AST = buildASTFromCodeWithArgs(SourcePath, Args);

		for (int Arg = 0; Arg < Args.size(); Arg++)
			delete[] ClangOptions[Arg];
		delete[] ClangOptions;

		// Set generated code and error message
		GeneratedSource = CodeGenerator.GetGeneratedCode();
		ErrorMsg = CodeGenerator.GetErrorMsg();

		// Zero is success code
		if (Result != 0 || CodeGenerator.ProducedError())
			return false;
		else
			return true;
	}

	
}

int main(int ArgC, char** ArgV)
{
	int32_t NumIncludes = 0;
	std::vector<std::string> IncludeDirs;
	std::string SourcePath;
	std::string IncludePath;
	std::string ModuleNameCaps;
	std::string GenOutPath;

	std::string GeneratedSource;
	std::string ErrorMsg;

	if (ArgC < 6)
	{
		std::cout << "Too few arguments" << std::endl;
		return 1;
	}

	SourcePath = ArgV[1];
	IncludePath = ArgV[2];
	ModuleNameCaps = ArgV[3];
	GenOutPath = ArgV[4];
	NumIncludes = std::atoi(ArgV[5]);

	for (int32_t Include = 0; Include < NumIncludes; Include++)
		IncludeDirs.push_back(ArgV[6 + Include]);

	bool bResult = Ry::GenerateReflectionCode(ModuleNameCaps,
		SourcePath,
		IncludePath,
		IncludeDirs,
		GeneratedSource,
		ErrorMsg,
		0
	);

	if (!ErrorMsg.empty())
	{
		std::cout << ErrorMsg << std::endl;
	}
	else if (bResult)
	{
		std::ofstream OutFile(GenOutPath);
		{
			OutFile << GeneratedSource << std::endl;
		}
		OutFile.close();
	}

	if (bResult)
		return 0;
	else
		return 1;
}