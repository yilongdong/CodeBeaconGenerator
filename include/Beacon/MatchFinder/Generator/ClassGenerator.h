#pragma once
#include <unordered_set>
#include <clang/AST/Type.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/ASTContext.h>

#include <Beacon/Model/CXXModel.h>

namespace CodeBeacon {

class ClassGenerator {
public:
  static std::unique_ptr<cxxmodel::Class> Create(clang::CXXRecordDecl const& clsDecl);
  static cxxmodel::Access toAccessModel(clang::AccessSpecifier const& accessSpecifier);
  static cxxmodel::Class::Method toMethodModel(clang::CXXMethodDecl const& methodDecl);
  static cxxmodel::Class::Method::Parameter toParameterModel(clang::ParmVarDecl const& paramDecl);
  static cxxmodel::Class::Field toFieldModel(clang::FieldDecl const& fieldDecl);
  static cxxmodel::SourceLocation toSourceLocationModel(clang::SourceManager const& sourceManager, clang::SourceLocation const& sourceLocation);
  static std::string toString(clang::QualType const &type, clang::ASTContext const&ctx);
  static std::string toString(const clang::RecordType &type, const clang::ASTContext &ctx);
  static cxxmodel::Namespace getTagNamespace(clang::TagDecl const& declaration);
  static std::string getTagName(const clang::TagDecl &declaration);
};

}