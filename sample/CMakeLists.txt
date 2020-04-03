cmake_minimum_required(VERSION 3.10)

project(sample)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

find_package(lmengine REQUIRED)
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
  lmengine::lmengine
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
add_custom_command(
  TARGET sample_editor_plugin
  POST_BUILD
  COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
  $<TARGET_FILE:sample_editor_plugin>
  ${CMAKE_CURRENT_SOURCE_DIR}/libgame${CMAKE_SHARED_LIBRARY_SUFFIX}
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
