#include <iostream>
#include <fstream>
#include <filesystem>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include <gflags/gflags.h>

#include "Beacon/ASTConsumer/CodeBeaconASTConsumer.h"

using namespace clang;

class CodeBeaconGenFrontendAction : public clang::ASTFrontendAction {
public:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) override {
    return std::make_unique<CodeBeacon::CodeBeaconASTConsumer>(Compiler);
  }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");

DEFINE_string(project, "", "C++项目路径");
DEFINE_string(compdb, "", "compile-command.json路径");
DEFINE_string(output, "", "日志输出的路径");
int main(int argc, char **argv) {
  google::SetUsageMessage(" usage ... ");
  google::SetVersionString("0.1.0");
  google::ParseCommandLineFlags(&argc, &argv, true);

  std::string LLVMProjectBase = "/Users/dongyilong/clang-llvm/llvm-project";
  std::string projDir = LLVMProjectBase +  "/" + "clang-tools-extra/code-beacon-gen";
  std::string compdb = LLVMProjectBase + "/" + "build";
  std::string output = LLVMProjectBase + "/" + "clang-tools-extra/code-beacon-gen/result.log";
  std::cout << google::SetCommandLineOption("project", projDir.c_str()) << '\n';
  std::cout << google::SetCommandLineOption("compdb", compdb.c_str()) << '\n';
  std::cout << google::SetCommandLineOption("output", output.c_str()) << '\n';

  std::string errorMsg;
  std::unique_ptr<clang::tooling::CompilationDatabase> db
      = clang::tooling::CompilationDatabase::loadFromDirectory(FLAGS_compdb, errorMsg);
  if (db == nullptr) {
    std::cerr << errorMsg << '\n';
    exit(0);
  }


  std::vector<std::string> sourceFileList{};
  std::filesystem::path projectDir{ FLAGS_project };
  using RecursiveIter = std::filesystem::recursive_directory_iterator;
  for (auto const& entry : RecursiveIter(projectDir)) {
    if (entry.path().extension() == ".cpp" || entry.path().extension() == ".cc") {
      sourceFileList.push_back(entry.path().string());
    }
  }
  for (auto const& path : sourceFileList) {
    std::cout << "source file = " << path << '\n';
  }

  std::ofstream out(FLAGS_output);
  std::cout.rdbuf(out.rdbuf());

  clang::tooling::ClangTool tool{ *db.get(), sourceFileList };
  return tool.run(clang::tooling::newFrontendActionFactory<CodeBeaconGenFrontendAction>().get());
}