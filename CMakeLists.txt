cmake_minimum_required(VERSION 3.16)
project(imperative_compiler LANGUAGES CXX)

set(COMPILER_CORE_SOURCES
    src/Lexer.cpp
    src/Parser.cpp
    src/Optimizer.cpp
    src/IR.cpp
    src/RegAlloc.cpp
    src/CodeGen.cpp
)

add_executable(compiler src/main.cpp ${COMPILER_CORE_SOURCES})

target_include_directories(compiler PRIVATE include)
target_compile_features(compiler PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(compiler PRIVATE /W4)
else()
    target_compile_options(compiler PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(compiler_tests tests/compiler_tests.cpp ${COMPILER_CORE_SOURCES})
    target_include_directories(compiler_tests PRIVATE include)
    target_compile_features(compiler_tests PRIVATE cxx_std_17)
    add_test(NAME compiler_tests COMMAND compiler_tests)
endif()
