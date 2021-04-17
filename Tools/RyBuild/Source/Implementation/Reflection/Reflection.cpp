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

			GeneratedSource << "template<>" << std::endl;
			GeneratedSource << "const Class*" << std::endl;
			GeneratedSource << "GetClassImpl(ClassTag<" << Record.Name << ">)" << std::endl;
			GeneratedSource << "{" << std::endl;
			{
				// Declare class
				GeneratedSource << "static Class C;" << std::endl;

				// Reflected class name
				GeneratedSource << "C.Name = \"" << Record.Name << "\";" << std::endl;

				// Resize fields member
				GeneratedSource << "C.Fields.Resize(" << Record.Fields.size() << ");" << std::endl;

				// Reflect all fields
				for(int FieldIndex = 0; FieldIndex < Record.Fields.size(); FieldIndex++)
				{
					const FieldDecl* Decl = Record.Fields[FieldIndex];
					std::string FieldType = Decl->getType()->getTypeClassName();
					std::string FieldsVar = "C.Fields[" + std::to_string(FieldIndex) + "]";
					
					GeneratedSource << FieldsVar << ".Type = GetType<" << FieldType << ">();" << std::endl;
					GeneratedSource << FieldsVar << ".Name = \"" << Decl->getNameAsString() << "\";" << std::endl;
					GeneratedSource << FieldsVar << ".Offset = offsetof(" << Record.Name << ", " << Decl->getNameAsString() << ");" << std::endl;					
				}
			}
			GeneratedSource << "}" << std::endl << std::endl;
		}
		
		virtual void run(const MatchFinder::MatchResult& Result)
		{
			const Decl* Declaration = Result.Nodes.getNodeAs<clang::Decl>("id");

			// Records (structs/unions/classes)
			const CXXRecordDecl* AsRecord = dynamic_cast<const CXXRecordDecl*>(Declaration);
			const FieldDecl* AsField = dynamic_cast<const FieldDecl*>(Declaration);
			const FunctionDecl* AsFunction = dynamic_cast<const FunctionDecl*>(Declaration);

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

						if(AnnotationAttrib->getAnnotation().str() == "reflect")
						{
							bReflectDecl = true;
						}

					}

					++AttribItr;
				}
			}

			if(bReflectDecl)
			{
				if (AsRecord)
				{
					// Start new record
					ReflectedRecord NewRecord;
					NewRecord.Name = AsRecord->getNameAsString();

					ReflectedRecords.push_back(NewRecord);
				}

				if(!AsFunction && !AsField)
				{
					std::cerr << "Unexpected reflection declaration " << Declaration->getDeclKindName() << std::endl;
					return;
				}

				if(ReflectedRecords.size() > 0)
				{
					ReflectedRecord& CurRecord = ReflectedRecords[ReflectedRecords.size() - 1];

					if (AsField)
					{
						CurRecord.Fields.push_back(AsField);
					}
					else if (AsFunction)
					{
						CurRecord.Functions.push_back(AsFunction);
					}
					
				}
				else if(AsFunction)
				{
					std::cerr << "Unexpected function before record " << AsFunction->getNameAsString() << std::endl;
				}
				else if(AsField)
				{
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
			GeneratedSource << "#include \"Reflection/Reflection.h\"" << std::endl;
			
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

	bool GenerateReflectionCode(std::string SourcePath, std::vector<std::string> Includes, std::string& GeneratedSource)
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