#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"

#include <memory>
namespace CodeBeacon {

//
class CodeBeaconMatchFinder {
public:
  CodeBeaconMatchFinder(clang::CompilerInstance& CI);
  ~CodeBeaconMatchFinder();
  void matchAST(clang::ASTContext &Context);
private:
  void loadMatherAndCallback();
  using ASTMatchFinderType = clang::ast_matchers::MatchFinder;
  using CallbackType = clang::ast_matchers::MatchFinder::MatchCallback;

  clang::CompilerInstance& CI;
  std::vector<std::unique_ptr<CallbackType>> callbacks;
  ASTMatchFinderType matchFinder;
};

} // namespace CodeBeacon