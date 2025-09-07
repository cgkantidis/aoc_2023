cmake_minimum_required(VERSION 3.20..4.1.1)

project(aoc_2023 VERSION 1.0.0 LANGUAGES CXX)

# generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(functions.cmake REQUIRED)

include(FetchContent)
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
find_package(libassert REQUIRED)
find_package(bshoshany_thread_pool REQUIRED)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(utility src/utility.cpp)
target_link_libraries(utility PRIVATE compilation_options sanitizer_options libassert::assert)

add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${bshoshany_thread_pool_SOURCE_DIR}/include)

foreach(day RANGE 1 25)
  foreach(part RANGE 1 2)
    if(day LESS 10)
      set(target d0${day}p${part})
    else()
      set(target d${day}p${part})
    endif()
    add_aoc_target(${target})
  endforeach()
endforeach()

