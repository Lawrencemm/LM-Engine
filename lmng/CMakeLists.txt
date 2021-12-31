project(lmng)

add_library(
  lmng
  SHARED

  src/reflect_component.cpp
  src/yaml_save_load.cpp
  src/bullet_physics/bullet_physics.cpp
  src/bullet_physics/rigid_body.cpp
  src/bullet_physics/character_controller.cpp
  src/transform.cpp
  src/name.cpp
  src/pose.cpp
  src/animation.cpp
  src/extents.cpp
  src/hierarchy.cpp
  src/logging.cpp
  src/context.cpp
  src/any_component.cpp
  src/archetype.cpp
  src/meta/clone.cpp
  src/meta/signal.cpp
  src/loaders/archetype.cpp
  src/asset_cache.cpp
  src/loaders/animation.cpp
  src/bullet_physics/constraints.cpp)

target_include_directories(
  lmng

  PUBLIC
  include
)

target_link_libraries(
  lmng

  PUBLIC
  lmlib
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
  test/test_pose.cpp
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
