#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>
#include "Beacon/MatchFinder/Generator/ClassGenerator.h"

using namespace CodeBeacon;


std::unique_ptr<cxxmodel::Class> ClassGenerator::Create(
  clang::CXXRecordDecl const& clsDecl,
  clang::SourceManager const& sourceManager,
  clang::SourceLocation const& sourceLocation) {
  std::unique_ptr<cxxmodel::Class> clsModel = std::make_unique<cxxmodel::Class>();

  clsModel->ns = getTagNamespace(clsDecl);
  clsModel->name = clsDecl.getNameAsString();
  clsModel->is_struct = clsDecl.isStruct();
  clsModel->id = clsDecl.getID();
  clsModel->source_location = toSourceLocationModel(sourceManager, sourceLocation);

  for (clang::CXXMethodDecl const* method : clsDecl.methods()) {
    if (method && (!method->isDefaulted() || method->isExplicitlyDefaulted())) {
      clsModel->methods.push_back(toMethodModel(*method, sourceManager));
    }
  }
  for (clang::CXXBaseSpecifier const& baseDecl : clsDecl.bases()) {
    clsModel->bases.push_back(toBaseModel(baseDecl, clsDecl));
  }
  return clsModel;
}

cxxmodel::Access ClassGenerator::toAccessModel(clang::AccessSpecifier const& accessSpecifier) {
  switch (accessSpecifier) {
  case clang::AccessSpecifier::AS_public:
    return cxxmodel::Access::PUBLIC;
  case clang::AccessSpecifier::AS_protected:
    return cxxmodel::Access::PROTECTED;
  case clang::AccessSpecifier::AS_private:
    return cxxmodel::Access::PRIVATE;
  default:
    return cxxmodel::Access::UNKNOWN;
  }
}

cxxmodel::Class::Method ClassGenerator::toMethodModel(clang::CXXMethodDecl const& methodDecl, clang::SourceManager const& sourceManager) {
  cxxmodel::Class::Method methodModel;
  methodModel.name = methodDecl.getNameAsString();
  methodModel.access = toAccessModel(methodDecl.getAccess());
  methodModel.type = toString(methodDecl.getReturnType(), methodDecl.getASTContext());
  methodModel.is_pure_virtual = methodDecl.isPure();
  methodModel.is_virtual = methodDecl.isVirtual();
  methodModel.is_const = methodDecl.isConst();
  methodModel.is_defaulted = methodDecl.isExplicitlyDefaulted();
  methodModel.is_static = methodDecl.isStatic();
  methodModel.is_implicit = methodDecl.isImplicit();
  methodModel.is_delete = methodDecl.isDeleted();
  methodModel.is_deprecated = methodDecl.isDeprecated();

  // ParmVarDecl
  for (clang::ParmVarDecl const* param : methodDecl.parameters()) {
    if (param) {
      methodModel.parameters.push_back(toParameterModel(*param, sourceManager));
    }
  }
  return methodModel;
}
cxxmodel::Class::Method::Parameter ClassGenerator::toParameterModel(clang::ParmVarDecl const& paramDecl, clang::SourceManager const& sourceManager) {
  cxxmodel::Class::Method::Parameter param;
  param.name = paramDecl.getNameAsString();
  param.type = toString(paramDecl.getType(), paramDecl.getASTContext());
  if (paramDecl.hasDefaultArg()) {
    clang::Expr const* defaultArg = paramDecl.getDefaultArg();
    if (defaultArg) {
      param.default_value = getSourceText(defaultArg->getSourceRange(), sourceManager);
    }
  }
  return param;
}

cxxmodel::Class::Field ClassGenerator::toFieldModel(clang::FieldDecl const& fieldDecl) {
  cxxmodel::Class::Field fieldModel;
  fieldModel.access = toAccessModel(fieldDecl.getAccess());
  fieldModel.name = fieldDecl.getNameAsString();
  fieldModel.type = toString(fieldDecl.getType(), fieldDecl.getASTContext());
  return fieldModel;
}
cxxmodel::SourceLocation ClassGenerator::toSourceLocationModel(
    clang::SourceManager const& sourceManager,
    clang::SourceLocation const& sourceLocation) {
  cxxmodel::SourceLocation sourceLocationModel;
  if (sourceLocation.isValid()) {
    sourceLocationModel.file = sourceManager.getFilename(sourceLocation).str();
    sourceLocationModel.line = sourceManager.getSpellingLineNumber(sourceLocation);
  }
  return sourceLocationModel;
}

cxxmodel::Class::Base ClassGenerator::toBaseModel(clang::CXXBaseSpecifier const& baseDecl, clang::CXXRecordDecl const& clsDecl) {
  cxxmodel::Class::Base baseModel;
  baseModel.name = toString(baseDecl.getType(), clsDecl.getASTContext());
  if (baseDecl.getType()->getAs<clang::RecordType>()) {
    // id = baseDecl.getType()->getAs<clang::RecordType>()->getDecl()
  }
  baseModel.access = toAccessModel(baseDecl.getAccessSpecifier());
  baseModel.is_virtual = baseDecl.isVirtual();
  return baseModel;


}

std::string ClassGenerator::toString(const clang::QualType &type, const clang::ASTContext &ctx) {
  clang::PrintingPolicy printingPolicy(ctx.getLangOpts());
  std::string result{type.getAsString(printingPolicy)};
  return result.empty() ? "(anonymous)" : result;
}

std::string ClassGenerator::toString(const clang::RecordType &type, const clang::ASTContext &ctx) {
  return toString(type.desugar(), ctx);
}

cxxmodel::Namespace ClassGenerator::getTagNamespace(const clang::TagDecl &declaration) {
  cxxmodel::Namespace ns;

  clang::DeclContext const *parent{declaration.getParent()};

  // ??????????????????????????????
  while (parent && !parent->isNamespace()) {
    parent = parent->getParent();
  }

  // ??????????????????????????????????????????
  while (parent && parent->isNamespace()) {
    const auto *ns_decl = static_cast<const clang::NamespaceDecl *>(parent);
    if (!ns_decl->isInline() && !ns_decl->isAnonymousNamespace())
      ns.push_back(ns_decl->getNameAsString());
    parent = parent->getParent();
  }
  std::reverse(ns.begin(), ns.end());
  return ns;
}

std::string ClassGenerator::getTagName(const clang::TagDecl &declaration) {
  auto base_name = declaration.getNameAsString();
  if (base_name.empty()) {
    // lambda???????????????struct??????????????????
    base_name = "(anonymous_" + std::to_string(declaration.getID()) + ")";
  }

  // ??????????????????????????????plantUML??????????????????????????????##????????????????????????
  // ?????????????????????Record?????????????????????class???struct??????
  if (declaration.getParent() && declaration.getParent()->isRecord()) {
    std::vector<std::string> name_tokens;
    name_tokens.push_back(base_name);
    clang::DeclContext const *cls_parent{declaration.getParent()};
    while (cls_parent->isRecord()) {
      auto parent_name = static_cast<const clang::RecordDecl *>(cls_parent) ->getNameAsString();
      name_tokens.push_back(parent_name);
      cls_parent = cls_parent->getParent();
    }
    std::reverse(name_tokens.begin(), name_tokens.end());
    // plantUML?????????????????????????????????##??????????????????????????????
    return cxxmodel::join(name_tokens.cbegin(), name_tokens.cend(), "##");
  }
  return base_name;
}

std::string ClassGenerator::getSourceTextRaw(clang::SourceRange range, clang::SourceManager const& sourceManager) {
  return clang::Lexer::getSourceText(
             clang::CharSourceRange::getCharRange(range),
             sourceManager,
             clang::LangOptions()
             ).str();
}

/**
 * SourceLocation?????? http://spiritsaway.info/clang-basic.html#6c9f9e
 * https://clang.llvm.org/doxygen/classclang_1_1Lexer.html#ae504b980c4435513f71c826a90df2bee
#define min(x,y) (x)>(y)?(y):(x)

void main(int argc,char**argv){

#line 4
    int a=min(2,4)
}
???????????????2????????????
Spelling Location ?????????????????? ???1???
Expansion Location ???????????????????????????????????? ???6???
Presumed Location ??????#line????????????????????????????????? ???4???
????????????????????????????????????????????????????????????????????????
 * */
std::string ClassGenerator::getSourceText(clang::SourceRange range, clang::SourceManager const& sourceManager) {
  clang::LangOptions langOptions;
  clang::SourceLocation startLocation = sourceManager.getSpellingLoc(range.getBegin());
  clang::SourceLocation lastTokenLocation = sourceManager.getSpellingLoc(range.getEnd());
  clang::SourceLocation stopLocation = clang::Lexer::getLocForEndOfToken(
      lastTokenLocation,0,sourceManager,langOptions);
  clang::SourceRange printableRange = clang::SourceRange{startLocation, stopLocation};
  return getSourceTextRaw(printableRange, sourceManager);
}