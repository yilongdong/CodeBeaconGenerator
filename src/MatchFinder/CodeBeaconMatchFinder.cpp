#include "Beacon/MatchFinder/CodeBeaconMatchFinder.h"
#include "Beacon/MatchFinder/Callback/ClassMatchCallback.h"
#include "Beacon/MatchFinder/Callback/FuncCallFuncExprCallback.h"

using namespace CodeBeacon;

CodeBeaconMatchFinder::CodeBeaconMatchFinder(clang::CompilerInstance& CI) : CI(CI) {
  loadMatherAndCallback();
}

CodeBeaconMatchFinder::~CodeBeaconMatchFinder() {}

void CodeBeaconMatchFinder::matchAST(clang::ASTContext &Context) {
  matchFinder.matchAST(Context);
}

void CodeBeaconMatchFinder::loadMatherAndCallback() {
  // 已经定义的类/union/struct匹配
  static clang::ast_matchers::DeclarationMatcher const recordMatcher =
      clang::ast_matchers::cxxRecordDecl(clang::ast_matchers::isDefinition()).bind("id");
  // 简单的函数调用
  static clang::ast_matchers::StatementMatcher callExprMatcher =
      clang::ast_matchers::callExpr(
          clang::ast_matchers::hasAncestor(clang::ast_matchers::functionDecl().bind("caller")),
          clang::ast_matchers::callee(clang::ast_matchers::functionDecl().bind("callee")));
  // matchFinder不会获取callback所有权, callback生命周期由callbacks管理。
  callbacks.emplace_back(new Callback::ClassMatchCallback(CI));
  matchFinder.addMatcher(recordMatcher, callbacks.back().get());
  callbacks.emplace_back(new Callback::FuncCallFuncExprCallback(CI));
  matchFinder.addMatcher(callExprMatcher, callbacks.back().get());
}
