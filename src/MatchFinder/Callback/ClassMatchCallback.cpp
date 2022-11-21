#include <iostream>

#include "Beacon/MatchFinder/Callback/ClassMatchCallback.h"
#include "Beacon/MatchFinder/Generator/ClassGenerator.h"

namespace CodeBeacon {
namespace Callback {
std::unordered_set<int64_t> ClassMatchCallback::classIDSet;
std::unordered_set<size_t> ClassMatchCallback::fileIDSet;

void outputLog(cxxmodel::Class const& cls) {
  using namespace nlohmann;
  std::cerr << "class = " << cls.name << " id = " << cls.id << " location = " << json{cls.source_location}.dump() <<'\n';
  std::cout << json{cls}.dump() << '\n';
  for (auto const& field : cls.fields) {
    std::cerr << "\tfield = " << json{field}.dump() << '\n';
  }
  for(auto const& method : cls.methods) {
    std::cerr << "\tmethod = " << json{method}.dump() << '\n';
  }
  for (auto const& base : cls.bases) {
    std::cerr << "\tbase = " << json{base}.dump() << '\n';
  }
}
void ClassMatchCallback::run(clang::ast_matchers::MatchFinder::MatchResult const& Result) {
  clang::CXXRecordDecl const* pClsDecl = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");
  if (shouldSkip(pClsDecl)) {
    return;
  }

  if (pClsDecl->isCompleteDefinition()) {
    std::unique_ptr<cxxmodel::Class> clsModel =
        ClassGenerator::Create(
            *pClsDecl,
            source_manager(), pClsDecl->getLocation()); // 后续改成用FullSourceLoc传参

    outputLog(*clsModel.get());
  }
}
bool ClassMatchCallback::shouldSkip(clang::CXXRecordDecl const* pClsDecl) {
  if(!pClsDecl) return true;
  if (source_manager().isInSystemHeader(pClsDecl->getSourceRange().getBegin())) { return true; }
  std::string filename = getFilenameOfNode(pClsDecl);
  if (!isUserSourceCode(filename)) return true;

  size_t fileID = std::hash<std::string>{}(filename);
  if (fileIDSet.find(fileID) != fileIDSet.end()) {
    return true;
  }
  fileIDSet.insert(fileID);
  // 单独classID不能确保不重复
  int64_t classID = pClsDecl->getID();
  if (classIDSet.find(classID) != classIDSet.end()) {
    return true;
  }
  classIDSet.insert(classID);

  return false;
}

} // namespace Callback
} // namespace CodeBeacon