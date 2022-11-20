#include <iostream>

#include "Beacon/MatchFinder/Callback/FuncCallFuncExprCallback.h"

namespace CodeBeacon {
namespace Callback {

void FuncCallFuncExprCallback::run(clang::ast_matchers::MatchFinder::MatchResult const& Result) {
      auto node = Result.Nodes.getNodeAs<clang::FunctionDecl>("caller");
      //    if (!node) return;
      std::string filename = CI.getSourceManager().getFilename(node->getSourceRange().getBegin()).str();
      if (!isUserSourceCode(filename)) return;

      // 获取调用者的函数定义
  //    std::cout << "call expr in " << filename << '\n';
      if (const clang::FunctionDecl *caller = Result.Nodes.getNodeAs<clang::FunctionDecl>("caller")) {
  //
  //      caller->dump();
      }
      // 获取被调用者的函数定义
      if (const clang::FunctionDecl *callee = Result.Nodes.getNodeAs<clang::FunctionDecl>("callee")) {
  //      callee->dump();
      }
}

} // namespace Callback
} // namespace CodeBeacon