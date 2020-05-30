cmake_minimum_required(VERSION 3.8)

project(lmlib)
set(
  LMLIB_LIST_DIR
  ${CMAKE_CURRENT_LIST_DIR}
  CACHE INTERNAL "lmlib top-level directory")

include(cmake/cpp_standard.cmake)

include(cmake/dependencies.cmake)

if (MSVC)
  set(
    COMPILER_DEFINITIONS

    _USE_MATH_DEFINES NOMINMAX
  )
  set(
    COMPILER_INTERFACE_OPTIONS

    /wd4250 /wd4008 /wd4996
  )
elseif(MINGW)
  set(
    COMPILER_DEFINITIONS

    _WIN32_WINNT=0x0602
  )
endif ()

set(
  SOURCES

  ${PLATFORM_SOURCES}

  src/geometry/frustum.cpp
  src/geometry/geometry.cpp

  src/graphics/camera.cpp

  src/util/remote_executor.cpp
  src/geometry/intersection.cpp
)

set(
  INCLUDE_DIRS

  include
  ${TBB_INCLUDE_DIRS}
  ${eigen_INCLUDE_DIRS}
  ${fmt_INCLUDE_DIRS}
)

set(
  LIBS

  ${fmt_LIBRARIES}
  ${TBB_LIBRARIES}
  Boost::Boost
)

set(
  DEFINITIONS
  "-DFMT_NO_FMT_STRING_ALIAS=0"
)

set(
  INTERFACE_DEFINITIONS

  ${COMPILER_INTERFACE_DEFINITIONS}
)

set(
  OPTIONS

  ${COMPILER_OPTIONS}
)

set(
  INTERFACE_OPTIONS

  ${COMPILER_INTERFACE_OPTIONS}
)

add_library(
  lmlib
  SHARED

  ${SOURCES}
)

target_include_directories(
  lmlib

  PUBLIC
  ${INCLUDE_DIRS}

  PRIVATE
  ${scope_guard_INCLUDES}
)

target_compile_definitions(
  lmlib

  PUBLIC
  ${DEFINITIONS}

  INTERFACE
  ${INTERFACE_DEFINITIONS}
)

target_compile_options(
  lmlib

  PUBLIC
  ${OPTIONS}

  INTERFACE
  ${INTERFACE_OPTIONS}
)

target_link_libraries(
  lmlib

  PUBLIC
  ${LIBS}
)

set_property(TARGET lmlib PROPERTY POSITION_INDEPENDENT_CODE ON)

install(
  TARGETS lmlib
  LIBRARY DESTINATION bin
  RUNTIME DESTINATION bin
)

# Testing
add_executable(
  test_lmlib

  test/test_lmlib.cpp
  src/geometry/test_geometry.cpp
  test/test_intersection.cpp
)

target_include_directories(
  test_lmlib

  PRIVATE
  ${Catch2_INCLUDE_DIRS}
)

target_link_libraries(
  test_lmlib

  lmlib
)

add_test(NAME lmlib COMMAND test_lmlib)
