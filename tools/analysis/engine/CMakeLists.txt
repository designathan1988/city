# The feature tracer's native engine (log/trace-engine-plan.md): the new core's analysis over integer nodes and
# objects, reading the input Python writes (featuretrace/engine_input.py). The compiler is CMake's choice (the CXX
# environment variable, CMake's convention); a MinGW build links statically so the program runs without the
# toolchain's runtime beside it, with a stack as deep as the analysis's recursion needs.
cmake_minimum_required(VERSION 3.28)
project(ftengine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(FTENGINE_STACK_BYTES 268435456)  # 256 MiB, as the Python side's deep thread

add_library(ftengine_core STATIC src/terms.cpp src/pickle_reader.cpp src/dump.cpp src/solver.cpp src/input.cpp
  src/analysis.cpp src/trees.cpp src/refs.cpp src/names.cpp src/members.cpp src/typesys.cpp src/values.cpp
  src/declared.cpp src/summaries.cpp src/protocols.cpp src/jsx.cpp src/texts.cpp src/transfers.cpp src/output.cpp
  src/objects.cpp src/diagnostics.cpp src/cycles.cpp src/guards.cpp src/verdicts.cpp src/copies.cpp src/by_key.cpp)
target_include_directories(ftengine_core PUBLIC src)
target_compile_options(ftengine_core PUBLIC -Wall -Wextra)

add_executable(ftengine src/main.cpp)
target_link_libraries(ftengine PRIVATE ftengine_core)

add_executable(ftengine_solver_test tests/solver_test.cpp)
target_link_libraries(ftengine_solver_test PRIVATE ftengine_core)

if(MINGW)
  foreach(target ftengine ftengine_solver_test)
    target_link_options(${target} PRIVATE -static "-Wl,--stack,${FTENGINE_STACK_BYTES}")
  endforeach()
endif()
