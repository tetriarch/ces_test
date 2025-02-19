cmake_minimum_required(VERSION 3.28)
project(ces_test VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)

FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(magic_enum)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS  src/**.cpp)


add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ces_test>/assets)

add_executable(ces_test ${SRC_FILES})

target_link_libraries(ces_test PRIVATE 
    nlohmann_json::nlohmann_json
    magic_enum::magic_enum
)

add_dependencies(ces_test copy_assets)
