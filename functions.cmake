if(CMAKE_CONFIGURATION_TYPES)
  list(APPEND CMAKE_CONFIGURATION_TYPES ASAN MSAN TSAN UBSAN PROFILE HEAP)
    list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
        "Add the configurations that we need"
        FORCE)
endif()

add_library(compilation_options INTERFACE)
target_compile_features(compilation_options INTERFACE cxx_std_23)
# enable all warnings
# https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#gcc--clang
target_compile_options(compilation_options INTERFACE
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wmisleading-indentation
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    $<$<CXX_COMPILER_ID:GNU>:
      -Wduplicated-branches
      -Wduplicated-cond
      -Wlogical-op
      -Wuseless-cast>)
# treat all warnings as errors
target_compile_options(compilation_options INTERFACE -Werror)

# https://clang.llvm.org/docs/AddressSanitizer.html
# https://clang.llvm.org/docs/ThreadSanitizer.html
# https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
# gperftools CPU Profiler https://github.com/gperftools/gperftools
add_library(sanitizer_options INTERFACE)
target_compile_options(sanitizer_options INTERFACE
    $<$<CONFIG:ASAN>:-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline -g -O1>
    $<$<CONFIG:MSAN>:-fsanitize=memory -fPIE -fsanitize-memory-track-origins -fno-inline -g -O1>
    $<$<CONFIG:TSAN>:-fsanitize=thread -fno-inline -g -O1>
    $<$<CONFIG:UBSAN>:-fsanitize=undefined,bounds -g -O1>
    $<$<CONFIG:PROFILE>:-g -O1>
    $<$<CONFIG:HEAP>:-g -O1>)

target_link_options(sanitizer_options INTERFACE
  $<$<CONFIG:ASAN>:-fsanitize=address>
  $<$<CONFIG:MSAN>:-fsanitize=memory -pie>
  $<$<CONFIG:TSAN>:-fsanitize=thread>
  $<$<CONFIG:UBSAN>:-fsanitize=undefined,bounds>
  $<$<CONFIG:PROFILE>:-lprofiler>
  $<$<CONFIG:HEAP>:-ltcmalloc>)

function(add_aoc_target target)
  if(EXISTS src/${target}.cpp)
    add_executable(${target} src/${target}.cpp)
    target_link_libraries(${target} PRIVATE utility compilation_options sanitizer_options libassert::assert BS_thread_pool)
  endif()
endfunction()
