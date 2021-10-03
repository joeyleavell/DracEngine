#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>
#include "Reflection/Reflection.h"
#include <fstream>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

namespace Ry
{
	enum RecordType
	{
		Class,
		Struct,
		Union
	};
	
	struct ReflectedRecord
	{
		RecordType Type;
		std::string Name;
		
		std::vector<const FieldDecl*> Fields;
		std::vector<const FunctionDecl*> Functions;
	};

	class ReflectionCodeGenerator : public MatchFinder::MatchCallback
	{
	public:

		void GenerateReflectedRecord(const ReflectedRecord& Record)
		{
			GeneratedSource << "namespace Ry" << std::endl;
			GeneratedSource << "{" << std::endl;
			{
				GeneratedSource << "\ttemplate<>" << std::endl;
				GeneratedSource << "\tinline const Ry::ReflectedClass*" << std::endl;
				GeneratedSource << "\tGetClassImpl(Ry::ClassTag<class " << Record.Name << ">)" << std::endl;
				GeneratedSource << "\t{" << std::endl;
				{
					// Declare class
					GeneratedSource << "\t\tstatic Ry::ReflectedClass C;" << std::endl;

					// Reflected class name
					GeneratedSource << "\t\tC.Name = \"" << Record.Name << "\";" << std::endl;

					// Resize fields member
					GeneratedSource << "\t\tC.Fields.Resize(" << Record.Fields.size() << ");" << std::endl;

					// Reflect all fields
					for (int FieldIndex = 0; FieldIndex < Record.Fields.size(); FieldIndex++)
					{
						const FieldDecl* Decl = Record.Fields[FieldIndex];
						std::string FieldType = Decl->getType().getAsString();
						std::string FieldsVar = "C.Fields[" + std::to_string(FieldIndex) + "]";

						GeneratedSource << "\t\t" << FieldsVar << ".Type = GetType<" << FieldType << ">();" << std::endl;
						GeneratedSource << "\t\t" << FieldsVar << ".Name = \"" << Decl->getNameAsString() << "\";" << std::endl;
						GeneratedSource << "\t\t" << FieldsVar << ".Offset = offsetof(" << Record.Name << ", " << Decl->getNameAsString() << ");" << std::endl;
					}

					GeneratedSource << "\t\treturn &C;" << std::endl;

				}
				GeneratedSource << "\t}" << std::endl << std::endl;
			}
			GeneratedSource << "}" << std::endl;


		}
		
		virtual void run(const MatchFinder::MatchResult& Result)
		{
			const Decl* Declaration = Result.Nodes.getNodeAs<clang::Decl>("id");

			bool bReflectDecl = false;

			// Look for reflection annotation
			if(Declaration->hasAttrs())
			{
				Decl::attr_iterator AttribItr = Declaration->attr_begin();

				while (AttribItr != Declaration->attr_end())
				{
					Attr* Attrib = *AttribItr;

					if (Attrib->getKind() == attr::Annotate)
					{
						AnnotateAttr* AnnotationAttrib = static_cast<AnnotateAttr*>(Attrib);

						if(AnnotationAttrib->getAnnotation().str() == "Reflect")
						{
							bReflectDecl = true;
						}

					}

					++AttribItr;
				}
			}

			if(bReflectDecl)
			{
				bool bIsRecord = Declaration->getKind() == Decl::CXXRecord;
				bool bIsFunction = Declaration->getKind() == Decl::Function;
				bool bIsField = Declaration->getKind() == Decl::Field;

				if (bIsRecord)
				{
					const CXXRecordDecl* AsRecord = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");

					//const CXXRecordDecl* AsRecord = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");

					// Start new record
					ReflectedRecord NewRecord;
					NewRecord.Name = AsRecord->getNameAsString();

					ReflectedRecords.push_back(NewRecord);
				}

				if(!bIsFunction && !bIsField)
				{
					std::cerr << "Unexpected reflection declaration " << Declaration->getDeclKindName() << std::endl;
					return;
				}

				if(ReflectedRecords.size() > 0)
				{
					ReflectedRecord& CurRecord = ReflectedRecords[ReflectedRecords.size() - 1];

					if (bIsField)
					{
						const FieldDecl* AsField = Result.Nodes.getNodeAs<clang::FieldDecl>("id");

						CurRecord.Fields.push_back(AsField);
					}
					else if (bIsFunction)
					{
						const FunctionDecl* AsFunction = Result.Nodes.getNodeAs<clang::FunctionDecl>("id");

						CurRecord.Functions.push_back(AsFunction);
					}
					
				}
				else if(bIsFunction)
				{
					const FunctionDecl* AsFunction = Result.Nodes.getNodeAs<clang::FunctionDecl>("id");

					std::cerr << "Unexpected function before record " << AsFunction->getNameAsString() << std::endl;
				}
				else if(bIsField)
				{
					const FieldDecl* AsField = Result.Nodes.getNodeAs<clang::FieldDecl>("id");

					std::cerr << "Unexpected field before record " << AsField->getNameAsString() << std::endl;
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

		std::string GetGeneratedCode()
		{
			return GeneratedSource.str();
		}
		
	private:

		std::vector<ReflectedRecord> ReflectedRecords;

		std::ostringstream GeneratedSource;

	};

	bool GenerateReflectionCode(std::string ModuleNameCaps, std::string SourcePath, std::vector<std::string> Includes, std::string& GeneratedSource)
	{
		
		MatchFinder Finder;
		ReflectionCodeGenerator CodeGenerator;

		DeclarationMatcher ClassMatcher
			= cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));

		DeclarationMatcher PropertyMatcher
			= fieldDecl(decl().bind("id"), hasAttr(attr::Annotate));

		DeclarationMatcher FunctionMatcher
			= functionDecl(decl().bind("id"), hasAttr(attr::Annotate));

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
		for(int Arg = 0; Arg < Args.size(); Arg++)
		{
			//std::cout << Args[Arg] << std::endl;
			ClangOptions[Arg] = new char[Args[Arg].size() + 1];
			strcpy(ClangOptions[Arg], Args[Arg].c_str());			
		}

		std::vector<std::string> Sources = { SourcePath };
		
		llvm::cl::OptionCategory MyToolCategory("my-tool options");
		clang::tooling::CommonOptionsParser Parser (ArgC, const_cast<const char**>(ClangOptions), MyToolCategory);
		clang::tooling::ClangTool Tool (Parser.getCompilations(), Sources);
		//Tool.appendArgumentsAdjuster(Parser.getArgumentsAdjuster());

		
		std::unique_ptr<FrontendActionFactory> Fac = newFrontendActionFactory(&Finder);
		int Result = Tool.run(Fac.get());

		//int Result = runToolOnCodeWithArgs(Fac->create(), SourcePath, Args);

		//std::unique_ptr<ASTUnit> AST = buildASTFromCodeWithArgs(SourcePath, Args);
		
		delete[] ClangOptions;

		// Return generated code
		GeneratedSource = CodeGenerator.GetGeneratedCode();

		// Zero is success code
		if(Result == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
}