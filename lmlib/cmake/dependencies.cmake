cmake_minimum_required(VERSION 3.10)

include(cmake/download_project.cmake)
include(cmake/cpp_standard.cmake)

# Support conan cmake_find_package generator. It puts Find* files in the
# binary directory.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MODULE_PATH})

find_package(Catch2 REQUIRED)
find_package(TBB REQUIRED)
find_package(scope_guard REQUIRED)
find_package(eigen REQUIRED)
find_package(fmt REQUIRED)
