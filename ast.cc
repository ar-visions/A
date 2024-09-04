#include "clang/Frontend/CompilerInstance.h"
int main(int argc, const char **argv) { clang::CompilerInstance ci; return 0; }

// g++ -std=c++17 -fno-rtti `llvm-config-14 --cxxflags --ldflags --libs all` -o my_ast_tool my_ast_tool.cpp

//fails with: undefined reference to `clang::CompilerInstance::CompilerInstance(std::shared_ptr<clang::PCHContainerOperations>, clang::InMemoryModuleCache*)'
