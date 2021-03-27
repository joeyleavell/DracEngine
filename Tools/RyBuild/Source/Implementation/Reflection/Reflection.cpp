#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

#include <iostream>
#include "Reflection/Reflection.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

namespace Ry
{

	class ClassFinder : public MatchFinder::MatchCallback
	{
	public:
		virtual void run(const MatchFinder::MatchResult& Result)
		{
			
		}

		virtual void onStartOfTranslationUnit()
		{
			
		}

		virtual void onEndOfTranslationUnit()
		{
			
		}
	};

	void Test()
	{
		MatchFinder Finder;
		ClassFinder ClassFinder;

		DeclarationMatcher ClassMatcher
			= cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
		DeclarationMatcher PropertyMatcher
			= cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
		DeclarationMatcher FunctionMatcher
			= cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));

		Finder.addMatcher(ClassMatcher, &ClassFinder);
		Finder.addMatcher(PropertyMatcher, &ClassFinder);
		Finder.addMatcher(FunctionMatcher, &ClassFinder);

		std::unique_ptr<FrontendActionFactory> Fac = newFrontendActionFactory(&Finder);

		if(!clang::tooling::runToolOnCode(Fac->create(), ""))
		{
			std::cerr << "Failed" << std::endl;
		}

	}
	
}