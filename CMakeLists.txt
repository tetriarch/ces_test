cmake_minimum_required(VERSION 3.28)
project(ces_test VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

include(ExternalProject)
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
FetchContent_MakeAvailable(nlohmann_json)

# header only
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

fetch_dependency(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.8
)

fetch_dependency(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.4
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(vendor/imgui)

add_executable(ces_test ${SRC_FILES})
add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ces_test>/assets)

target_link_libraries(ces_test PRIVATE 
    nlohmann_json::nlohmann_json
    magic_enum::magic_enum
    SDL3::SDL3
    SDL3_image::SDL3_image
    imgui
)

target_compile_options(ces_test PRIVATE 
    $<$<CONFIG:Debug>: -g -DDEBUG -m64 -Wall>
    $<$<CONFIG:Release>: -O3 -DNDEBUG -m64 -Wall>
)

if (CMAKE_HOST_WIN32)
        add_custom_command(TARGET ces_test POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ces_test> $<TARGET_FILE_DIR:ces_test>
                COMMAND_EXPAND_LISTS
        )
endif()

# static link mingw stuff on release to freely deploy
if(CMAKE_HOST_WIN32)
target_link_options(ces_test PRIVATE
    $<$<CONFIG:Release>: -static -Wl,-Bstatic -lstdc++ -lgcc -lgcc_eh -lwinpthread>
)
endif()

add_dependencies(ces_test copy_assets)