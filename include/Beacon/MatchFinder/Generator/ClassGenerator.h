#pragma once
#include <unordered_set>
#include <clang/AST/Type.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/ASTContext.h>

#include "Beacon/Model/CXXModel.h"

namespace CodeBeacon {

// 实现参考
// https://github.com/bkryza/clang-uml
class ClassGenerator {
public:
  static std::unique_ptr<cxxmodel::Class> Create(
      clang::CXXRecordDecl const& clsDecl,
      clang::SourceManager const& sourceManager,
      clang::SourceLocation const& sourceLocation);
  static cxxmodel::Access toAccessModel(clang::AccessSpecifier const& accessSpecifier);
  static cxxmodel::Class::Method toMethodModel(clang::CXXMethodDecl const& methodDecl, clang::SourceManager const& sourceManager);
  static cxxmodel::Class::Method::Parameter toParameterModel(clang::ParmVarDecl const& paramDecl, clang::SourceManager const& sourceManager);
  static cxxmodel::Class::Field toFieldModel(clang::FieldDecl const& fieldDecl);
  static cxxmodel::SourceLocation toSourceLocationModel(clang::SourceManager const& sourceManager, clang::SourceLocation const& sourceLocation);
  static cxxmodel::Class::Base toBaseModel(clang::CXXBaseSpecifier const& baseDecl, clang::CXXRecordDecl const& clsDecl);
  static std::string toString(clang::QualType const &type, clang::ASTContext const&ctx);
  static std::string toString(const clang::RecordType &type, const clang::ASTContext &ctx);
  static cxxmodel::Namespace getTagNamespace(clang::TagDecl const& declaration);
  static std::string getTagName(const clang::TagDecl &declaration);
  static std::string getSourceTextRaw(clang::SourceRange range, clang::SourceManager const& sourceManager);
  static std::string getSourceText(clang::SourceRange range, clang::SourceManager const& sourceManager);

};

}