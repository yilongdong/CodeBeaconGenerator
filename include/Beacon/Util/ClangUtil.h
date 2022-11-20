#include "clang/AST/Decl.h"

#include "Beacon/Model/CXXModel.h"

cxxmodel::Namespace get_tag_namespace(clang::TagDecl const& declaration);