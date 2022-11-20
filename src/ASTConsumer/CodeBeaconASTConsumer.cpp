#include "Beacon/ASTConsumer/CodeBeaconASTConsumer.h"

namespace CodeBeacon {
  CodeBeaconASTConsumer::CodeBeaconASTConsumer(clang::CompilerInstance &CI) : matchFinder(CI) {}
  bool CodeBeaconASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef D) { return true; }
  void CodeBeaconASTConsumer::HandleTranslationUnit(clang::ASTContext &Context) {
    // 解析完的语法树传给matcher
    matchFinder.matchAST(Context);
  }
} // namespace CodeBeacon