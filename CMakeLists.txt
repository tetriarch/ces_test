cmake_minimum_required(VERSION 3.28)
project(ces_test VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(FetchDependency
  GIT_REPOSITORY https://github.com/jpetrie/fetch-dependency.git
  GIT_TAG origin/main
)

FetchContent_MakeAvailable(FetchDependency)
include(${fetchdependency_SOURCE_DIR}/FetchDependency.cmake)

# header only
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

# header only
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)

fetch_dependency(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.8
    CONFIGURATION ${CMAKE_BUILD_TYPE}
)

FetchContent_MakeAvailable(magic_enum)
FetchContent_MakeAvailable(nlohmann_json)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS  src/**.cpp)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ces_test>/assets)

add_executable(ces_test ${SRC_FILES})

target_link_libraries(ces_test PRIVATE 
    nlohmann_json::nlohmann_json
    magic_enum::magic_enum
    SDL3::SDL3
)

target_compile_options(ces_test PRIVATE 
    $<$<CONFIG:Debug>: -g -DDEBUG -m64 -Wall>
    $<$<CONFIG:Release>: -O3 -DNDEBUG -m64 -Wall>
)

if(CMAKE_HOST_WIN32)
target_link_options(ces_test PRIVATE
    $<$<CONFIG:Release>: -static -Wl,-Bstatic -lstdc++ -lgcc -lgcc_eh -lwinpthread>
)
endif()

add_dependencies(ces_test copy_assets)
