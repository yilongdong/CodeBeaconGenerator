#pragma once
#include <unordered_set>
#include <clang/AST/Decl.h>
#include "Beacon/MatchFinder/Callback/MatchCallbackBase.h"
#include "Beacon/Model/CXXModel.h"

namespace CodeBeacon {
namespace Callback {

class ClassMatchCallback : public MatchCallbackBase {
public:
  ClassMatchCallback(clang::CompilerInstance &CI) : MatchCallbackBase(CI) {}
  virtual ~ClassMatchCallback() = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override;
  bool shouldSkip(clang::CXXRecordDecl const* pClsDecl);

  static std::unordered_set<int64_t> classIDSet;
  static std::unordered_set<size_t> fileIDSet;
};

} // namespace Callback
} // namespace CodeBeacon
