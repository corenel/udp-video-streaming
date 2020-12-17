cmake_minimum_required(VERSION 3.4)

project( udp_video_streaming )

# set compilation
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-Wall -Wextra -Wno-unused-parameter -O2)

# useful macros
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake)

# find dependencies
find_package( OpenCV REQUIRED )

# common include directories
include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

# find all source files
file(GLOB_RECURSE
    src_files
    ${PROJECT_SOURCE_DIR}/src/*.cc
    ${PROJECT_SOURCE_DIR}/src/*/*.cc
    )

# common library
add_library(
    ${PROJECT_NAME}
    SHARED
    ${src_files}
)
target_link_libraries(
    ${PROJECT_NAME}
    ${OpenCV_LIBS}
)

# add applications
add_simple_apps()
