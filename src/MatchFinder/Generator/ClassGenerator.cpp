#include <clang/Basic/SourceManager.h>
#include <Beacon/MatchFinder/Generator/ClassGenerator.h>

using namespace CodeBeacon;


std::unique_ptr<cxxmodel::Class> ClassGenerator::Create(clang::CXXRecordDecl const& clsDecl) {
    std::unique_ptr<cxxmodel::Class> clsModel = std::make_unique<cxxmodel::Class>();
    clsModel->ns = getTagNamespace(clsDecl);
    clsModel->name = clsDecl.getNameAsString();
    clsModel->is_struct = clsDecl.isStruct();
    clsModel->id = clsDecl.getID();

    for (auto const* method : clsDecl.methods()) {
      if (method) {
        clsModel->methods.push_back(toMethodModel(*method));
      }
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

cxxmodel::Class::Method ClassGenerator::toMethodModel(clang::CXXMethodDecl const& methodDecl) {
  cxxmodel::Class::Method methodModel;
  methodModel.name = methodDecl.getNameAsString();
  methodModel.access = toAccessModel(methodDecl.getAccess());
  methodModel.type = toString(methodDecl.getReturnType(), methodDecl.getASTContext());
  methodModel.is_pure_virtual = methodDecl.isPure();
  methodModel.is_virtual = methodDecl.isVirtual();
  methodModel.is_const = methodDecl.isConst();
  methodModel.is_defaulted = methodDecl.isDefaulted();
  methodModel.is_static = methodDecl.isStatic();
  // ParmVarDecl
  for (clang::ParmVarDecl const* param : methodDecl.parameters()) {
    if (param) {
      methodModel.parameters.push_back(toParameterModel(*param));
    }
  }
  return methodModel;
}
cxxmodel::Class::Method::Parameter ClassGenerator::toParameterModel(clang::ParmVarDecl const& paramDecl) {
  cxxmodel::Class::Method::Parameter param;
  param.name = paramDecl.getNameAsString();
  param.type = toString(paramDecl.getType(), paramDecl.getASTContext());
  return param;
}

cxxmodel::Class::Field ClassGenerator::toFieldModel(clang::FieldDecl const& fieldDecl) {
    cxxmodel::Class::Field fieldModel;
    fieldModel.access = toAccessModel(fieldDecl.getAccess());
    fieldModel.name = fieldDecl.getNameAsString();
    fieldModel.type = toString(fieldDecl.getType(), fieldDecl.getASTContext());
    return fieldModel;
}
cxxmodel::SourceLocation ClassGenerator::toSourceLocationModel(clang::SourceManager const& sourceManager, clang::SourceLocation const& sourceLocation) {
  cxxmodel::SourceLocation sourceLocationModel;
  if (sourceLocation.isValid()) {
    sourceLocationModel.file = sourceManager.getFilename(sourceLocation).str();
    sourceLocationModel.line = sourceManager.getSpellingLineNumber(sourceLocation);
  }
  return sourceLocationModel;
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

  auto *parent{declaration.getParent()};

  // First walk up to the nearest namespace, e.g. from nested class or enum
  while (parent && !parent->isNamespace()) {
    parent = parent->getParent();
  }

  // Now build up the namespace
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
    base_name = "(anonymous_" + std::to_string(declaration.getID()) + ")";
  }

  if (declaration.getParent() && declaration.getParent()->isRecord()) {
    // 此处用来处理类嵌套，plantUML不支持嵌套类，所以用##表示类的嵌套关系
    // If the record is nested within another record (e.g. class or struct)
    // we have to maintain a containment namespace in order to ensure
    // unique names within the diagram
    std::vector<std::string> name_tokens;
    name_tokens.push_back(base_name);
    auto *cls_parent{declaration.getParent()};
    while (cls_parent->isRecord()) {
      auto parent_name = static_cast<const clang::RecordDecl *>(cls_parent) ->getNameAsString();
      name_tokens.push_back(parent_name);
      cls_parent = cls_parent->getParent();
    }
    std::reverse(name_tokens.begin(), name_tokens.end());
    return cxxmodel::join(name_tokens.cbegin(), name_tokens.cend(), "##");
  }
  return base_name;
}