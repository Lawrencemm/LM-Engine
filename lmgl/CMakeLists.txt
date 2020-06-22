cmake_minimum_required(VERSION 3.8)

project(lmgl)

# Support conan cmake_find_package generator. It puts Find* files in the
# binary directory.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MODULE_PATH})

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(Vulkan REQUIRED)
find_package(lmlib REQUIRED)
find_package(lmpl REQUIRED)

set(
  SOURCES

  src/create_renderer.cpp

  src/resource_sink.cpp

  src/vulkan/buffer/vulkan_buffer.cpp
  src/vulkan/vulkan_window_stage.cpp
  src/vulkan/vulkan_renderer.cpp
  src/vulkan/vulkan_exceptions.cpp
  src/vulkan/geometry/vulkan_geometry.cpp
  src/vulkan/vulkan_frame.cpp
  src/vulkan/buffer/vulkan_buffer_base.cpp
  src/vulkan/vulkan_texture.cpp
  src/vulkan/material.cpp
  src/vulkan/vulkan_texture_stage.cpp
  src/vulkan/stage_helpers.cpp
)

add_library(
  lmgl
  SHARED

  ${SOURCES}
  src/vulkan/vulkan_debug_report_ext.cpp
)

target_include_directories(
  lmgl

  INTERFACE
  include

  PRIVATE
  include/lmgl
  ${Vulkan_INCLUDE_DIRS}
  ${lmpl_INCLUDES}
)

target_link_libraries(
  lmgl

  lmlib::lmlib
  Vulkan::Vulkan
  lmpl::lmpl
  ${fmt_LIBRARIES}
)

target_compile_definitions(
  lmgl

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

if (WIN32)
  set(PLATFORM_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR)
elseif (UNIX)
  set(
    PLATFORM_DEFINITIONS

    VK_USE_PLATFORM_XCB_KHR
    VK_USE_PLATFORM_WAYLAND_KHR
  )
endif()

target_compile_definitions(
  lmgl

  PUBLIC
  VK_PROTOTYPES
  "-DFMT_NO_FMT_STRING_ALIAS=0"
  ${PLATFORM_DEFINITIONS}
)

target_link_directories(
  lmgl
  PRIVATE

  ${PLATFORM_LINK_DIRS}
)
target_include_directories(
  lmgl

  PUBLIC
  ${INCLUDE_DIRS}
  include
)

install(
  TARGETS lmgl
  LIBRARY DESTINATION bin
  RUNTIME DESTINATION bin
)

add_executable(
  test_lmgl

  src/vulkan/test/mock_vulkan_api.cpp
  src/test_lmgl.cpp
)

target_include_directories(
  test_lmgl

  PRIVATE
  ${Catch2_INCLUDE_DIRS}
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
  test_lmgl

  ${fmt_LIBRARIES}
)
