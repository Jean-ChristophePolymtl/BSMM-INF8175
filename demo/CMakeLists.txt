cmake_minimum_required(VERSION 3.22)

project(demo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Don't build raylib's examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

find_package(Eigen3 REQUIRED)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE raylib
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Eigen3::Eigen
)