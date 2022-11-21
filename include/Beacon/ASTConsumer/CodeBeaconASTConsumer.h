#pragma once
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/DeclGroup.h>
#include <clang/Frontend/CompilerInstance.h>

#include "Beacon/MatchFinder/CodeBeaconMatchFinder.h"

namespace CodeBeacon {

class CodeBeaconASTConsumer : public clang::ASTConsumer {
public:
  explicit CodeBeaconASTConsumer(clang::CompilerInstance &CI);
  bool HandleTopLevelDecl(clang::DeclGroupRef D) override;
  void HandleTranslationUnit(clang::ASTContext &Context) override;
private:
  CodeBeaconMatchFinder matchFinder;
};

} // namespace CodeBeacon
