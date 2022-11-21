#include "Beacon/Util/ClangUtil.h"
#include "Beacon/Model/CXXModel.h"
#include <algorithm>
// 需要删除
cxxmodel::Namespace get_tag_namespace(clang::TagDecl const &declaration) {
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