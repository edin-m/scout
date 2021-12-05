# root

cmake_minimum_required(VERSION 3.8)
project(Scout)

set(CMAKE_CXX_STANDARD 11)

set(GIT_ROOT ${PROJECT_SOURCE_DIR})
set(BIN_ROOT ${GIT_ROOT}/bin)
set(SRC_ROOT ${GIT_ROOT}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_ROOT})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_ROOT})

include(cmake/macros.cmake)

include_directories(src)

add_subdirectory(src)
add_subdirectory(projects)
#add_subdirectory(external)


