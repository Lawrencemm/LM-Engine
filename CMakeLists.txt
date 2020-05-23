cmake_minimum_required(VERSION 3.10)

project(workspace)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conanworkspace.cmake)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /permissive-")
  add_compile_definitions("_USE_MATH_DEFINES" "NOMINMAX")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

enable_testing()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_STANDARD 17)
  add_compile_options(-Werror=return-type)
else ()
  set(CMAKE_CXX_STANDARD 20)
endif ()

conan_workspace_subdirectories()
