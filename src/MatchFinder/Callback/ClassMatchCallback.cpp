#include <iostream>

#include "Beacon/MatchFinder/Callback/ClassMatchCallback.h"
#include "Beacon/MatchFinder/Generator/ClassGenerator.h"
#include "Beacon/Util/ClangUtil.h"

namespace CodeBeacon {
namespace Callback {
std::unordered_set<size_t> ClassMatchCallback::fileIDSet;
std::unordered_set<int64_t> ClassMatchCallback::classIDSet;

void ClassMatchCallback::run(clang::ast_matchers::MatchFinder::MatchResult const& Result) {
  clang::CXXRecordDecl const* pClsDecl = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");
  if(pClsDecl == nullptr) { return; }
  if (source_manager().isInSystemHeader(pClsDecl->getSourceRange().getBegin())) { return; }
  std::string filename = source_manager().getFilename(pClsDecl->getSourceRange().getBegin()).str();
  if (!isUserSourceCode(filename)) return;
  if (filename.empty()) return;
//  if (filename.find("code-beacon-gen") == std::string::npos) {
//    return;
//  }

  // 减少重复分析
  size_t fileID = std::hash<std::string>{}(filename);
  if (fileIDSet.find(fileID) != fileIDSet.end()) {
    return;
  }
  fileIDSet.insert(fileID);
  int64_t classID = pClsDecl->getID();
  if (classIDSet.find(classID) != classIDSet.end()) {
    return;
  }
  classIDSet.insert(classID);

  if (pClsDecl->isCompleteDefinition()) {
    auto clsModel = ClassGenerator::Create(*pClsDecl);
    clsModel->source_location = ClassGenerator::toSourceLocationModel(source_manager(), pClsDecl->getLocation());
    nlohmann::json laocationJson = clsModel->source_location;
    std::cout << "class = " << clsModel->name << " id = " << clsModel->id << " location = " << laocationJson.dump() <<'\n';

    for (auto const& field : clsModel->fields) {
      nlohmann::json fieldJson = field;
      std::cout << "\tfield = " << fieldJson.dump() << '\n';
    }

    for(auto const& method : clsModel->methods) {
      nlohmann::json methodJson = method;
      std::cout << "\tmethod = " << methodJson.dump() << '\n';
    }
  }
}

} // namespace Callback
} // namespace CodeBeacon