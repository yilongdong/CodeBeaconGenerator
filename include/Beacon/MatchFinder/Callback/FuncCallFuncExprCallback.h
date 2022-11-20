#pragma once
#include "Beacon/MatchFinder/Callback/MatchCallbackBase.h"
#include "Beacon/Model/CXXModel.h"

namespace CodeBeacon {
namespace Callback {

class FuncCallFuncExprCallback : public MatchCallbackBase {
public:
  FuncCallFuncExprCallback(clang::CompilerInstance &CI) : MatchCallbackBase(CI) {}
  virtual ~FuncCallFuncExprCallback() = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override;
};
} // namespace Callback
} // namespace CodeBeacon