# cmake -G Ninja -S /Users/dongyilong/clang-llvm/llvm-project/llvm -B /Users/dongyilong/clang-llvm/llvm-project/release-build/clang-tools-extra -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=RelWithDebInfo
# ninja xxx
set(LLVM_LINK_COMPONENTS support)


include_directories(include)

############################################################
# Create a Beacon Model library
############################################################
add_library(BeaconModel src/Model/CXXModel.cpp)
add_library(Beacon::Model ALIAS BeaconModel)
target_compile_options(BeaconModel PRIVATE -fno-rtti)


############################################################
# Create a Beacon Util library
############################################################
add_library(BeaconUtil src/Util/ClangUtil.cpp)
add_library(Beacon::Util ALIAS BeaconUtil)
target_compile_options(BeaconUtil PRIVATE -fno-rtti)
#target_link_libraries(BeaconUtil PUBLIC Beacon::Model)


############################################################
# Create a Beacon MatchFinder library
############################################################
file(GLOB_RECURSE BeaconMatchFinderSRCS src/MatchFinder "*.cpp" "*.cc" "*.c" "*.h" "*.hpp" "*.inl")
add_library(BeaconMatchFinder ${BeaconMatchFinderSRCS})
add_library(Beacon::MatchFinder ALIAS BeaconMatchFinder)
target_compile_options(BeaconMatchFinder PRIVATE -fno-rtti)
#target_link_libraries(BeaconMatchFinder PUBLIC Beacon::Util)


############################################################
# Create a Beacon ASTConsumer library
############################################################
file(GLOB_RECURSE BeaconASTConsumerSRCS src/ASTConsumer "*.cpp" "*.cc" "*.c" "*.h" "*.hpp" "*.inl")
add_library(BeaconASTConsumer ${BeaconASTConsumerSRCS})
add_library(Beacon::ASTConsumer ALIAS BeaconASTConsumer)
target_compile_options(BeaconASTConsumer PRIVATE -fno-rtti)
#target_link_libraries(BeaconASTConsumer PUBLIC Beacon::MatchFinder)

############################################################
# Create a Beacon standalone CLI tool
############################################################
set(ClangLLVMLibs clangAST clangBasic clangFrontend clangSerialization clangTooling)
add_clang_executable(code-beacon-gen standalone/main.cpp)
target_link_libraries(code-beacon-gen
        PRIVATE
        ${ClangLLVMLibs}
        Beacon::Model
        Beacon::Util
        Beacon::MatchFinder
        Beacon::ASTConsumer)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    add_custom_target( docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
