cmake_minimum_required(VERSION 3.21)

project(sample)

set(CMAKE_INSTALL_RPATH "\$ORIGIN")

add_library(
  sample_interface
  INTERFACE
)
target_link_libraries(
  sample_interface
  INTERFACE
  lmng
  lmtk
  spdlog::spdlog
)

set(
  SAMPLE_SOURCES
  src/simulations/rigid_bodies_battle.cpp
  src/reflect_types.cpp
  src/simulations/character_movement.cpp
  src/simulations/many_characters.cpp
  src/system/robot_system.cpp
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

  lmeditor
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
  lmhuv
  SDL2::SDL2
)

target_compile_definitions(
  sample_standalone

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

target_include_directories(
  sample_standalone
  PRIVATE
  ${SDL2_INCLUDE_DIRS}
)

set_target_properties(
  sample_standalone
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

lm_runtime_sync(sample_standalone)

add_executable(
  test_sample

  src/test_sample.cpp

  ${SAMPLE_SOURCES}
)

target_link_libraries(
  test_sample

  PRIVATE
  sample_interface
  Catch2::Catch2
)

lm_runtime_sync(test_sample)

LM_PACKAGE(sample_standalone)
