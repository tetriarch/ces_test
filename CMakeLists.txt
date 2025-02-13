cmake_minimum_required(VERSION 3.28)
project(ces_test VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS  src/**.cpp)

add_executable(ces_test ${SRC_FILES})

