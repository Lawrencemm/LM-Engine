cmake_minimum_required(VERSION 3.10)

project(workspace)

include(cmake/utils.cmake)

set(
  LM_ENGINE_LOGGING_LEVEL
  0
  CACHE STRING
  "spdlog logging level for all lm-engine projects, see spdlog logging levels documentation."
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(EnTT REQUIRED)
find_package(Bullet REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Catch2 REQUIRED)
find_package(TBB REQUIRED)
find_package(scope_guard REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost REQUIRED)
find_package(Vulkan REQUIRED)
find_package(embed-resource REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenMesh REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(clara REQUIRED)
find_package(range-v3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(glslangValidator IMPORTED)
set_target_properties(glslangValidator PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/bin/glslangValidator)
add_executable(embed-resource IMPORTED)
set_target_properties(embed-resource PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/bin/embed-resource)

include(${CMAKE_CURRENT_BINARY_DIR}/scripts/embed-resource.cmake)

add_subdirectory(lmlib)
add_subdirectory(lmng)
add_subdirectory(lmgl)
add_subdirectory(lmpl)
add_subdirectory(lmtk)
add_subdirectory(lmhuv)
add_subdirectory(lmeditor)

include(cmake/package_project.cmake)
include(cmake/utils.cmake)

add_subdirectory(sample)

SUBDIRLIST(PROJECT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/projects)

foreach(subdir ${PROJECT_SUBDIRS})
  add_subdirectory(projects/${subdir})
endforeach()
