#include <iostream>
#include <filesystem>
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

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

int main(int argc, const char **argv) {
  llvm::Expected<clang::tooling::CommonOptionsParser> ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();

  std::vector<std::string> sourceFileList{OptionsParser.getSourcePathList()};
  std::filesystem::path projectDir{"/Users/dongyilong/clang-llvm/llvm-project/clang-tools-extra/code-beacon-gen"};
  for (auto const& entry : std::filesystem::recursive_directory_iterator(projectDir)) {
    if (entry.path().extension() == ".cpp" || entry.path().extension() == ".cc") {
      sourceFileList.push_back(entry.path().string());
    }
  }
  for (auto const& path : sourceFileList) {
    std::cout << "source file = " << path << '\n';
  }

  clang::tooling::ClangTool tool{OptionsParser.getCompilations(), sourceFileList };

  return tool.run(clang::tooling::newFrontendActionFactory<CodeBeaconGenFrontendAction>().get());
}