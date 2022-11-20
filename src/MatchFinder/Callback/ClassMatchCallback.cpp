#include <iostream>

#include "Beacon/MatchFinder/Callback/ClassMatchCallback.h"
#include "Beacon/Util/ClangUtil.h"

namespace CodeBeacon {
namespace Callback {

void ClassMatchCallback::run(clang::ast_matchers::MatchFinder::MatchResult const& Result) {
  clang::CXXRecordDecl const* pClsDecl = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");
  if(pClsDecl == nullptr) {
    return;
  }
  clang::SourceLocation begLocation = pClsDecl->getSourceRange().getBegin();
  // 跳过系统头文件
  if (source_manager().isInSystemHeader(begLocation)) {
//    std::cout << "skip system header\t";
    return;
  }

  std::string filename = source_manager().getFilename(begLocation).str();
  if (!isUserSourceCode(filename)) return;
  if (filename.empty()) return;
  if (filename.find("code-beacon-gen") == std::string::npos) {
    return;
  }
  if (pClsDecl->isCompleteDefinition()) {
   cxxmodel::Class clsModel;
   clsModel.ns = get_tag_namespace(*pClsDecl);
   clsModel.name = pClsDecl->getNameAsString();
   clsModel.is_struct = pClsDecl->isStruct();
   if (pClsDecl->getLocation().isValid()) {
     clsModel.source_location.file = source_manager().getFilename(pClsDecl->getLocation()).str();
     clsModel.source_location.line = source_manager().getSpellingLineNumber(pClsDecl->getLocation());
   }
   for (auto const* methods : pClsDecl->methods()) {
     if (methods == nullptr) continue;
     processMethod(*methods, clsModel);
   }

   std::cout << "class = " << clsModel.name << '\n';
   for(auto const& method : clsModel.methods) {
     nlohmann::json methodJson = method;
     std::cout << "\tmethod = " << methodJson.dump() << '\n';
   }
//   nlohmann::json json = clsModel;
//   std::cout << json.dump() << '\n';
  }
}

void ClassMatchCallback::processMethod(clang::CXXMethodDecl const& methodDecl, cxxmodel::Class& clsModel) {
  if (methodDecl.isDefaulted() && !methodDecl.isExplicitlyDefaulted()) {
    return;
  }
  clsModel.methods.push_back({});
  cxxmodel::Class::Method& methodModel = clsModel.methods.back();
  methodModel.name = methodDecl.getNameAsString();
  methodModel.is_pure_virtual = methodDecl.isPure();
  methodModel.is_virtual = methodDecl.isVirtual();
  methodModel.is_const = methodDecl.isConst();
  methodModel.is_defaulted = methodDecl.isDefaulted();
  methodModel.is_static = methodDecl.isStatic();
  // ParmVarDecl
  for (auto const* param : methodDecl.parameters()) {
    if (param) {
      processFunctionParameter(*param, methodModel);
    }
  }
}

void ClassMatchCallback::processFunctionParameter(clang::ParmVarDecl const& paramDecl, cxxmodel::Class::Method& methodModel) {
  methodModel.parameters.push_back({});
  cxxmodel::Class::Method::Parameter& param = methodModel.parameters.back();
  param.name = paramDecl.getNameAsString();
}

} // namespace Callback
} // namespace CodeBeacon