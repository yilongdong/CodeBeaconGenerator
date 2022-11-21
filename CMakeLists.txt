# cmake -G Ninja -S /Users/dongyilong/clang-llvm/llvm-project/llvm -B /Users/dongyilong/clang-llvm/llvm-project/release-build/clang-tools-extra -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=RelWithDebInfo
# ninja xxx
set(LLVM_LINK_COMPONENTS support)


include_directories(include)
#include_directories(external/spdlog/include)


#find_package(spdlog CONFIG REQUIRED)

############################################################
# Create a Beacon Model library
############################################################
add_library(BeaconModel src/Model/CXXModel.cpp)
add_library(Beacon::Model ALIAS BeaconModel)
target_compile_options(BeaconModel PRIVATE -fno-rtti)
#target_link_libraries(BeaconModel PUBLIC ${SPDLOG_LIBS})


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
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external)
find_package(gflags REQUIRED)
include_directories (${gflags_INCLUDE_DIR})
set(ClangLLVMLibs clangAST clangBasic clangFrontend clangSerialization clangTooling)
add_clang_executable(code-beacon-gen standalone/main.cpp)
target_link_libraries(code-beacon-gen
        PRIVATE
        ${ClangLLVMLibs}
        gflags
        Beacon::Model
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
