cmake_minimum_required(VERSION 3.16)

project(stdiscm_project1
    VERSION 0.1.0
    DESCRIPTION "STDISCM Project 1 - threaded prime search, four variants"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Single-config generators (Make, Ninja) default to no optimization and no
# debug info unless a build type is picked, so choose a sane one.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Drop binaries in one predictable place regardless of generator.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(ENABLE_SANITIZERS "Build with ThreadSanitizer" OFF)

# One executable per variant, named after its directory. Sources are globbed so
# splitting a variant into main.cpp + ThreadWorker.cpp needs no edit here;
# CONFIGURE_DEPENDS makes the build re-glob when files are added or removed.
# Editor scratch copies ("main copy.cpp") are skipped - they usually hold a
# second main() and would break the link.
set(VARIANTS
    variant1_immediate_range
    variant2_immediate_divisibility
    variant3_deferred_range
    variant4_deferred_divisibility)

set(index 1)
foreach(variant IN LISTS VARIANTS)
    file(GLOB sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${variant}/*.cpp")
    list(FILTER sources EXCLUDE REGEX "copy")
    if(NOT sources)
        message(WARNING "${variant}: no .cpp files, skipping")
        continue()
    endif()

    add_executable(${variant} ${sources})
    # include/ holds the pieces shared by every variant; the variant's own
    # directory is on the path so "ThreadWorker.h" resolves without a prefix.
    target_include_directories(${variant} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/${variant}")
    target_link_libraries(${variant} PRIVATE Threads::Threads)

    if(MSVC)
        target_compile_options(${variant} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${variant} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(ENABLE_SANITIZERS AND NOT MSVC)
        target_compile_options(${variant} PRIVATE -fsanitize=thread -g -O1)
        target_link_options(${variant} PRIVATE -fsanitize=thread)
    endif()

    # `cmake --build build --target run2` builds variant 2 and runs it. The
    # working directory is the variant's own folder so it reads its config.txt.
    add_custom_target(run${index}
        COMMAND $<TARGET_FILE:${variant}>
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${variant}"
        USES_TERMINAL
        COMMENT "Running ${variant}")
    add_dependencies(run${index} ${variant})

    math(EXPR index "${index} + 1")
endforeach()

# The interactive "which variant?" prompt lives in the top-level Makefile
# (`make`), not here: CMake's configure step is run non-interactively by IDEs
# and CI, so a prompt at configure time would hang them.
