project(lmng)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(lmlib REQUIRED)
find_package(cereal REQUIRED)
find_package(EnTT REQUIRED)
find_package(Bullet REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Catch2 REQUIRED)

add_library(
  lmng
  SHARED

  src/reflection.cpp
  src/serialisation.cpp
  src/bullet_physics/bullet_physics.cpp
  src/bullet_physics/rigid_body.cpp
  src/bullet_physics/character_controller.cpp
  src/transform.cpp
  src/name.cpp
  src/pose.cpp
  src/animation.cpp
  src/extents.cpp
  src/hierarchy.cpp
  src/logging.cpp)

target_include_directories(
  lmng

  PUBLIC
  include
)

target_link_libraries(
  lmng

  PUBLIC
  lmlib::lmlib
  cereal::cereal
  EnTT::EnTT
  Bullet::Bullet
  yaml-cpp::yaml-cpp
)

target_compile_definitions(
  lmng

  PRIVATE
  SPDLOG_ACTIVE_LEVEL=${LM_ENGINE_LOGGING_LEVEL}
)

if (MSVC)
  target_compile_definitions(lmng PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE)
endif()

add_executable(
  test_lmng
  test/test_lmng.cpp
  test/test_pose
  test/test_hierarchy.cpp
  test/test_archetype.cpp
)

target_link_libraries(
  test_lmng
  PRIVATE
  Catch2::Catch2
  lmng
)

add_test(NAME lmng COMMAND test_lmng)

if(MINGW)
  target_compile_options(lmng PRIVATE "-Wa,-mbig-obj")
endif()
