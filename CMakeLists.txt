cmake_minimum_required(VERSION 3.16..3.31.2)

project(aoc_2023 VERSION 1.0.0 LANGUAGES CXX)

# set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  OVERRIDE_FIND_PACKAGE
)
FetchContent_Declare(
  scn
  GIT_REPOSITORY https://github.com/eliaskosunen/scnlib.git
  OVERRIDE_FIND_PACKAGE
)
FetchContent_Declare(
  libassert
  GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
  GIT_TAG main
  OVERRIDE_FIND_PACKAGE
)
FetchContent_Declare(
  bshoshany_thread_pool
  GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
  OVERRIDE_FIND_PACKAGE
)
find_package(fmt REQUIRED)
find_package(scn REQUIRED)
find_package(libassert REQUIRED)
find_package(bshoshany_thread_pool REQUIRED)

function(target_add_warnings target)
  # enable all warnings
  target_compile_options(${target} PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
  # https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#gcc--clang
  target_compile_options(${target} PRIVATE -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wmisleading-indentation -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${target} PRIVATE -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wuseless-cast)
  endif()
  # treat all warnings as errors
  #target_compile_options(${target} PRIVATE -Werror)
endfunction()

function(target_add_link_options target)
  if(CMAKE_BUILD_TYPE STREQUAL PPROF)
    target_link_libraries(${target} PRIVATE tcmalloc)
  elseif(CMAKE_BUILD_TYPE STREQUAL TSAN)
    target_link_options(${target} PRIVATE -fsanitize=thread)
  elseif(CMAKE_BUILD_TYPE STREQUAL MSAN)
    target_link_options(${target} PRIVATE -fsanitize=memory)
  endif()
endfunction()

set(allowedBuildTypes Debug Release ASAN MSAN UBSAN TSAN PPROF PERF)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")
if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
  message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "Debug build" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" CACHE STRING "Release build" FORCE)
set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O0" CACHE STRING "Address Sanitizer build" FORCE)
set(CMAKE_CXX_FLAGS_MSAN "-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O0" CACHE STRING "Memory Sanitizer build" FORCE)
set(CMAKE_CXX_FLAGS_UBSAN "-fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O0" CACHE STRING "Undefined Behaviour Sanitizer" FORCE)
set(CMAKE_CXX_FLAGS_TSAN "-fsanitize=thread -g -O0" CACHE STRING "Thread Sanitizer" FORCE)
set(CMAKE_CXX_FLAGS_PPROF "-g -O0" CACHE STRING "Heap Profiler" FORCE)
set(CMAKE_CXX_FLAGS_PERF "-g -O0" CACHE STRING "Linux profiling with performance counters" FORCE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "MSAN" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message(FATAL_ERROR "Memory sanitizer is not currently supported by gcc. Try clang instead.")
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(utility src/utility.cpp)
target_add_warnings(utility)
target_link_libraries(utility PRIVATE libassert::assert)

add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${bshoshany_thread_pool_SOURCE_DIR}/include)

function(add_aoc_target target)
  if(EXISTS src/${target}.cpp)
    add_executable(${target} src/${target}.cpp)
    target_add_warnings(${target})
    target_add_link_options(${target})
    # enable whole-program optimizations
    target_compile_options(${target} PRIVATE -fwhole-program)
    target_link_libraries(${target} PRIVATE fmt::fmt scn::scn libassert::assert utility BS_thread_pool)
  endif()
endfunction()

foreach(day RANGE 1 25)
  foreach(part RANGE 1 2)
    if (day LESS 10)
      add_aoc_target(d0${day}p${part})
    else()
      add_aoc_target(d${day}p${part})
    endif()
  endforeach()
endforeach()
