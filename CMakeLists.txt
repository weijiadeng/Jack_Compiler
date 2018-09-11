cmake_minimum_required(VERSION 3.10)
project(Jack_compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(Jack_compiler main.cpp CompilationEngine.h CompilationEngine.cpp JackTokenizer.cpp JackTokenizer.h SymbolTable.h SymbolTable.cpp stdafx.h VMWriter.h VMWriter.cpp)