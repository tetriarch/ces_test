cmake_minimum_required(VERSION 3.28)
project(ces_test VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
)

FetchContent_MakeAvailable(nlohmann_json)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS  src/**.cpp)

add_executable(ces_test ${SRC_FILES})

target_link_libraries(ces_test PRIVATE nlohmann_json::nlohmann_json)
