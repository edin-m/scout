# root

cmake_minimum_required(VERSION 3.10)
project(Scout)

set(CMAKE_CXX_STANDARD 11)

set(GIT_ROOT ${PROJECT_SOURCE_DIR})
set(BIN_ROOT ${GIT_ROOT}/bin)
set(SRC_ROOT ${GIT_ROOT}/src)
set(EXT_LIB_ROOT ${GIT_ROOT}/external/lib_bin)

# $<1: makes VS and QtCreator output to the same folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${BIN_ROOT}>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${BIN_ROOT}>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${BIN_ROOT}>)

include(cmake/macros.cmake)
include(cmake/link_dependencies.cmake)

set_arch()
set_build_type()

set(CMAKE_INSTALL_PREFIX ${BIN_ROOT})

include_directories(src)

add_subdirectory(src)
add_subdirectory(projects)
