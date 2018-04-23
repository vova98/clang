#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");

class IfStmtHandler : public MatchFinder::MatchCallback {
public:
  IfStmtHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // The matched 'if' statement was bound to 'ifStmt'.
    if (const IfStmt *IfS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
      const Stmt *Then = IfS->getThen();
      Rewrite.InsertText(Then->getLocStart(), "// the 'if' part\n", true, true);

      if (const Stmt *Else = IfS->getElse()) {
        Rewrite.InsertText(Else->getLocStart(), "// the 'else' part\n", true,
                           true);
      }
    }
  }

private:
  Rewriter &Rewrite;
};

class IncrementForLoopHandler : public MatchFinder::MatchCallback {
public:
  IncrementForLoopHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
  	//printf("forStrt\n");
		const Stmt *forL = Result.Nodes.getNodeAs<clang::Stmt>("forLoop");
		//printf("forMidl\n");
		Rewrite.InsertText(forL->getLocStart(), "/*mem*/", true, true);
		//printf("forEnd\n");
		//forL->dump();
  }


private:
  Rewriter &Rewrite;
};

class MainCall : public MatchFinder::MatchCallback {
public:
  MainCall(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
		//printf("asdasd\n");
		const Decl *MainC = Result.Nodes.getNodeAs<clang::Decl>("main");
		//MainC->dump();
  }


private:
  Rewriter &Rewrite;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : HandlerForIf(R), HandlerForFor(R), HandlerForMain(R)  {
    // Add a simple matcher for finding 'if' statements.
    Matcher.addMatcher(ifStmt().bind("ifStmt"), &HandlerForIf);

		Matcher.addMatcher(forStmt(isExpansionInMainFile(), 
															mlhasLoopInit("begin"), 
															anyOf(mlhasLoopInit("end"), hasCondition(hasDescendant(mlhasName("end")))) 
															).bind("forLoop"), &HandlerForFor);

		Matcher.addMatcher(functionDecl(hasName("main")).bind("main"), &HandlerForMain);
  }

  internal::Matcher<ForStmt> mlhasLoopInit(std::string str){
  	return hasLoopInit(hasDescendant(mlhasName(str)));
  }

  internal::Matcher<Stmt> mlhasName(std::string str){
  	return memberExpr(member(hasName(str)));
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    // Run the matchers when we have the whole TU parsed.
    Matcher.matchAST(Context);
  }

private:
  IfStmtHandler HandlerForIf;
  IncrementForLoopHandler HandlerForFor;
	MainCall HandlerForMain;
  MatchFinder Matcher;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, MatcherSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
