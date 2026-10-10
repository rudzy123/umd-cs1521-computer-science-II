cmake_minimum_required(VERSION 3.16)

project(cs1521
    VERSION 1.0.0
    DESCRIPTION "UMD CS 1521 Day 2: object-oriented programming and memory"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(CS1521_ENABLE_SANITIZERS
    "Instrument project targets with AddressSanitizer and UndefinedBehaviorSanitizer"
    OFF
)

function(cs1521_configure_target target)
    target_compile_options(${target} PRIVATE
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wextra>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wpedantic>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Werror>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wconversion>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wshadow>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wnon-virtual-dtor>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wold-style-cast>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wcast-align>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wunused>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Woverloaded-virtual>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wnull-dereference>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wdouble-promotion>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wformat=2>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wimplicit-fallthrough>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wduplicated-cond>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wduplicated-branches>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wlogical-op>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wuseless-cast>
    )

    if(CS1521_ENABLE_SANITIZERS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
            target_compile_options(${target} PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
            )
            target_link_options(${target} PRIVATE
                -fsanitize=address,undefined
            )
        else()
            message(FATAL_ERROR "CS1521_ENABLE_SANITIZERS requires GCC or Clang")
        endif()
    endif()
endfunction()

# Templates (the list, stack, queue, and binary search) live in headers.
# These translation units are the non-template types students can step through.
add_library(cs1521 STATIC
    src/managed_buffer.cpp
    src/recursion.cpp
    src/shape.cpp
)
add_library(cs1521::cs1521 ALIAS cs1521)
target_include_directories(cs1521 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(cs1521 PUBLIC cxx_std_20)
cs1521_configure_target(cs1521)

add_executable(cs1521_demo src/demo.cpp)
target_link_libraries(cs1521_demo PRIVATE cs1521::cs1521)
cs1521_configure_target(cs1521_demo)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(cs1521_tests
    tests/test_linked_list.cpp
    tests/test_stack.cpp
    tests/test_queue.cpp
    tests/test_shapes.cpp
    tests/test_recursion.cpp
    tests/test_managed_buffer.cpp
)
target_link_libraries(cs1521_tests PRIVATE cs1521::cs1521 Catch2::Catch2WithMain)
cs1521_configure_target(cs1521_tests)

enable_testing()
add_test(NAME cs1521_tests COMMAND cs1521_tests)
if(CS1521_ENABLE_SANITIZERS)
    set_tests_properties(cs1521_tests PROPERTIES
        ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1;UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1"
    )
endif()
