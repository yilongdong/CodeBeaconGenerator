#pragma once
#include "clang/AST/Decl.h"
#include "Beacon/MatchFinder/Callback/MatchCallbackBase.h"
#include "Beacon/Model/CXXModel.h"

namespace CodeBeacon {
namespace Callback {

class ClassMatchCallback : public MatchCallbackBase {
public:
  ClassMatchCallback(clang::CompilerInstance &CI) : MatchCallbackBase(CI) {}
  virtual ~ClassMatchCallback() = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override;

  void processMethod(clang::CXXMethodDecl const& methodDecl, cxxmodel::Class& clsModel);
  void processFunctionParameter(clang::ParmVarDecl const& paramDecl, cxxmodel::Class::Method& methodModel);
};

} // namespace Callback
} // namespace CodeBeacon
