cmake_minimum_required(VERSION 3.10)

project(sample)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(lmng REQUIRED)
find_package(lmeditor REQUIRED)
find_package(lmtk REQUIRED)
find_package(lmhuv REQUIRED)
find_package(Boost REQUIRED COMPONENTS dll)
find_package(Catch2 REQUIRED)


add_library(
  sample_interface
  INTERFACE
)
target_link_libraries(
  sample_interface
  INTERFACE
  lmng::lmng
  lmtk::lmtk
)

set(
  SAMPLE_SOURCES
  src/simulations/rigid_bodies_battle.cpp
  src/reflection.cpp
  src/simulations/character_movement.cpp
)


add_library(
  sample_editor_plugin
  SHARED

  src/editor_plugin.cpp
  ${SAMPLE_SOURCES}
)
set_target_properties(
  sample_editor_plugin
  PROPERTIES
  CXX_VISIBILITY_PRESET hidden
)
target_link_libraries(
  sample_editor_plugin

  lmeditor::lmeditor
  sample_interface
  Boost::Boost
)
target_include_directories(
  sample_editor_plugin

  PRIVATE
  ${lmeditor_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(
  sample_editor_plugin

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)


add_executable(
  sample_standalone

  ${SAMPLE_SOURCES}
  src/standalone.cpp
  src/application.cpp
)

target_link_libraries(
  sample_standalone

  sample_interface
  lmhuv::lmhuv
)

target_compile_definitions(
  sample_standalone

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

add_executable(
  test_sample

  src/test_sample.cpp

  src/simulations/character_movement.cpp
)

target_link_libraries(
  test_sample

  PRIVATE
  sample_interface
  Catch2::Catch2
)
