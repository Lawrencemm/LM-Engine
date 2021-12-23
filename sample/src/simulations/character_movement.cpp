#include "character_movement.h"
#include "../components/character_input.h"
#include "../components/robot.h"
#include "../system/robot_system.h"
#include <boost/geometry.hpp>
#include <iostream>
#include <lmlib/intersection.h>
#include <lmlib/math_constants.h>
#include <lmng/animation.h>
#include <lmng/camera.h>
#include <lmng/kinematic.h>
#include <lmng/name.h>
#include <lmng/simulation.h>
#include <lmng/transform.h>
#include <yaml-cpp/yaml.h>

character_movement::character
  character_movement::get_player_character(entt::registry &registry)
{
    auto character_controller_count = registry.size<character_input>();

    if (character_controller_count != 1)
        throw std::runtime_error{
          "One and only one character input is supported."};

    auto entity = registry.view<character_input>()[0];
    return character{
      entity,
      registry.get<lmng::transform>(entity),
      registry.get<lmng::character_controller>(entity),
    };
}

character_movement::character_movement(lmng::simulation_init const &init)
    : physics{lmng::create_physics(init.registry)},
      camera{init.registry.create()},
      animation_system{},
      hierarchy_system{init.registry},
      ground{lmng::find_entity(init.registry, "Ground")},
      swing_arms_animation{
        init.asset_cache.load<lmng::animation_data>("animation/swing_arms")}
{
    const character &player = get_player_character(init.registry);

    auto camera_transform = lmng::transform{Eigen::Vector3f{0.f, 2.5f, -7.5f}};
    lmng::look_at(camera_transform, {});
    init.registry.emplace<lmng::transform>(camera, camera_transform);
    init.registry.emplace<lmng::camera>(camera, 1.1f, 0.1f, 1000.f, true);
    init.registry.emplace<lmng::parent>(camera, player.entity);
}

void character_movement::handle_input_event(
  lmtk::input_event const &input_event,
  entt::registry &registry)
{
}

void character_movement::update(
  entt::registry &registry,
  float dt,
  lmtk::input_state const &input_state)
{
    auto character = get_player_character(registry);

    apply_movement_controls(character, registry, dt, input_state);

    move_robots(registry, ground);

    control_robots_animation(
      registry, physics, animation_system, swing_arms_animation);

    physics->step(registry, dt);

    animation_system.update(registry, dt);
}

void character_movement::apply_movement_controls(
  character &character,
  entt::registry &registry,
  float dt,
  lmtk::input_state const &input_state)
{
    float speed = 1.f;

    Eigen::Vector3f movement_direction{0.f, 0.f, 0.f};

    if (input_state.key_state[lmpl::key_code::S])
        movement_direction += Eigen::Vector3f{-speed, 0.f, 0.f};

    if (input_state.key_state[lmpl::key_code::F])
        movement_direction += Eigen::Vector3f{speed, 0.f, 0.f};

    if (input_state.key_state[lmpl::key_code::E])
        movement_direction += Eigen::Vector3f{0.f, 0.f, speed};

    if (input_state.key_state[lmpl::key_code::D])
        movement_direction += Eigen::Vector3f{0.f, 0.f, -speed};

    movement_direction.normalize();

    Eigen::Matrix3f const local_to_world =
      character.transform.rotation.toRotationMatrix();

    Eigen::Vector3f euler_angles = local_to_world.eulerAngles(0, 1, 2);

    Eigen::Vector3f const world_velocity = local_to_world * movement_direction;

    character.controller.requested_velocity = world_velocity;

    float frame_rotation = lm::pi * dt;
    float rotation_result{0.f};

    if (input_state.key_state[lmpl::key_code::Left])
    {
        rotation_result -= frame_rotation;
    }
    if (input_state.key_state[lmpl::key_code::Right])
    {
        rotation_result += frame_rotation;
    }

    if (rotation_result != 0.f)
    {
        auto transform = registry.get<lmng::transform>(character.entity);
        registry.replace<lmng::transform>(
          character.entity,
          lmng::transform{
            transform.position,
            transform.rotation *
              Eigen::AngleAxisf{rotation_result, Eigen::Vector3f::UnitY()}});
    }
}
