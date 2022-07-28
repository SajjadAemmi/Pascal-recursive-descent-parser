# CMakeList.txt : CMake project for Pascal-recursive-descent-parser, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("Pascal-recursive-descent-parser")

include_directories(headers)

file(GLOB SOURCES sources/*.cpp)

# target_include_directories(headers sources)

# set(sources
# file(GLOB SRC main.cpp sources/*.cpp headers/*.hpp)

# Add source to this project's executable.
add_executable(Pascal-recursive-descent-parser ${SOURCES})

# TODO: Add tests and install targets if needed.
