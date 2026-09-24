cmake_minimum_required(VERSION 3.16)

project(project1
    VERSION 0.1.0
    DESCRIPTION "STDISCM Project 1 - multithreaded C++"
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

# Add new .cpp files here as you create them.
add_executable(project1
    src/main.cpp
    src/ThreadWorker.cpp
)

target_include_directories(project1 PRIVATE include)
target_link_libraries(project1 PRIVATE Threads::Threads)

if(MSVC)
    target_compile_options(project1 PRIVATE /W4 /permissive-)
else()
    target_compile_options(project1 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Opt-in sanitizer build: cmake -B build -DENABLE_SANITIZERS=ON
option(ENABLE_SANITIZERS "Build with thread/address sanitizers" OFF)
if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(project1 PRIVATE -fsanitize=thread -g -O1)
    target_link_options(project1 PRIVATE -fsanitize=thread)
endif()
