project(lmengine)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_STANDARD 17)
else ()
  set(CMAKE_CXX_STANDARD 20)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(lmlib REQUIRED)
find_package(cereal REQUIRED)
find_package(EnTT REQUIRED)
find_package(Bullet REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Catch2 REQUIRED)

add_library(
  lmengine
  SHARED

  src/reflection.cpp
  src/serialisation.cpp
  src/bullet_physics/bullet_physics.cpp
  src/bullet_physics/rigid_body.cpp
  src/bullet_physics/character_controller.cpp
  src/transform.cpp
  src/name.cpp
)

target_include_directories(
  lmengine

  PUBLIC
  include
)

target_link_libraries(
  lmengine

  PUBLIC
  lmlib::lmlib
  cereal::cereal
  EnTT::EnTT
  Bullet::Bullet
  yaml-cpp::yaml-cpp
)

if (MSVC)
  target_compile_definitions(lmengine PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE)
endif()

add_executable(
  test_lmengine
  src/test_lmengine.cpp
  src/test_transform.cpp
)

target_link_libraries(
  test_lmengine
  PRIVATE
  Catch2::Catch2
  lmengine
)

add_test(NAME lmengine COMMAND test_lmengine)

if(MINGW)
  target_compile_options(lmengine PRIVATE "-Wa,-mbig-obj")
endif()
