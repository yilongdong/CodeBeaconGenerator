#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"

namespace CodeBeacon {
namespace Callback {

class MatchCallbackBase : public clang::ast_matchers::MatchFinder::MatchCallback {
protected:
  clang::CompilerInstance &CI;
  template <class NodeType> std::string getFilenameOfNode(NodeType node) {
    return CI.getSourceManager().getFilename(node->getSourceRange().getBegin()).str();
  }
  clang::SourceManager& source_manager() {
    return CI.getSourceManager();
  }
  virtual bool isUserSourceCode(std::string filename) const {
    return filename.find("/Library/Developer/CommandLineTools/") != 0;
  }

public:
  MatchCallbackBase(clang::CompilerInstance &CI) : CI(CI) {}
  virtual ~MatchCallbackBase() = default;
  void run(clang::ast_matchers::MatchFinder::MatchResult const& Result) override {}
};

} // namespace Callback
} // namespace CodeBeacon

