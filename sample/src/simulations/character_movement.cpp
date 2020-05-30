#include "character_movement.h"
#include "../components/character_input.h"
#include "../components/robot.h"
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
      ground{lmng::find_entity(init.registry, "Ground")},
      left_forward_pose{animation_system.load_pose(
        init.registry,
        "left_forward",
        YAML::LoadFile(
          (init.project_dir / "assets/pose/left_forward.lpose").string()))},
      right_forward_pose{animation_system.load_pose(
        init.registry,
        "right_forward",
        YAML::LoadFile(
          (init.project_dir / "assets/pose/right_forward.lpose").string()))},
      swing_arms_animation{animation_system.load_animation(YAML::LoadFile(
        (init.project_dir / "assets/animation/swing_arms.lmanim").string()))}
{
    init.registry.assign<lmng::transform>(camera);
    init.registry.assign<lmng::camera>(camera, 1.1f, 0.1f, 1000.f, true);
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

    move_robots(registry, dt);

    control_animation(registry, dt);

    physics->step(registry, dt);

    animation_system.update(registry, dt);

    camera_follow_character(registry, character);
}

void character_movement::camera_follow_character(
  entt::registry &registry,
  character_movement::character const &character)
{
    auto &camera_transform = registry.get<lmng::transform>(camera);

    camera_transform.position =
      character.transform.position +
      character.transform.rotation * Eigen::Vector3f{0.f, 2.5f, -7.5f};

    lmng::look_at(camera_transform, character.transform);
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

    if (input_state.key_state[lmpl::key_code::Left])
    {
        physics->rotate_character(
          registry,
          character.entity,
          Eigen::Vector3f{
            0.f,
            -lm::pi * dt,
            0.f,
          });
    }

    if (input_state.key_state[lmpl::key_code::Right])
    {
        physics->rotate_character(
          registry,
          character.entity,
          Eigen::Vector3f{
            0.f,
            lm::pi * dt,
            0.f,
          });
    }
}

void character_movement::control_animation(entt::registry &registry, float dt)
{
    registry.view<lmng::character_controller, robot>().each(
      [&](auto entity, auto &character_controller, auto) {
          auto current_velocity =
            physics->get_character_velocity(registry, entity);

          bool was_moving = current_velocity.squaredNorm() > 0.001f;

          bool started_moving =
            !was_moving &&
            character_controller.requested_velocity.squaredNorm() > 0.001f;

          bool stopped_moving =
            was_moving &&
            character_controller.requested_velocity.squaredNorm() < 0.001f;

          if (!was_moving)
          {
              auto maybe_animation_state =
                registry.try_get<lmng::animation_state>(entity);

              if (maybe_animation_state)
              {
                  auto &animation_state = *maybe_animation_state;

                  bool left_forward = animation_state.progress >= 0.5f;

                  if (
                    (animation_state.rate < 0.f && !left_forward) ||
                    (animation_state.rate > 0.f && left_forward))
                  {
                      registry.remove<lmng::animation_state>(entity);
                  }
              }
          }

          if (started_moving)
          {
              auto maybe_animation_state =
                registry.try_get<lmng::animation_state>(entity);

              if (maybe_animation_state)
              {
                  auto &animation_state = *maybe_animation_state;

                  bool left_forward = animation_state.progress >= 0.5f;

                  animation_state.rate = std::abs(animation_state.rate) *
                                         (left_forward ? 1.f : -1.f);
              }
              else
              {
                  animation_system.animate(
                    registry,
                    entity,
                    swing_arms_animation,
                    0.5f,
                    1.f,
                    lmng::anim_loop_type::pendulum);
              }
          }

          if (stopped_moving)
          {
              auto &animation_state =
                registry.get<lmng::animation_state>(entity);

              bool left_forward = animation_state.progress >= 0.5f;

              animation_state.rate =
                std::abs(animation_state.rate) * (left_forward ? -1.f : 1.f);
          }
      });
}

void character_movement::move_robots(entt::registry &registry, float dt)
{
    registry.view<lmng::character_controller, lmng::transform, robot>().each(
      [&](
        auto entity,
        auto const character_controller,
        auto const transform,
        auto) {
          auto ground_extents =
            registry.get<lmng::box_collider>(ground).extents;
          auto ground_transform = registry.get<lmng::transform>(ground);

          Eigen::Vector2f ground_extents2{ground_extents[0], ground_extents[2]};
          Eigen::Vector2f pos_ground_relative{
            transform.position.x() - ground_transform.position.x(),
            transform.position.z() - ground_transform.position.z()};

          if (boost::geometry::within(
                pos_ground_relative, lm::get_box(ground_extents2)))
          {
              auto [segment, distance] =
                lm::closest_edge(pos_ground_relative, ground_extents2);

              auto projected = transform.rotation * Eigen::Vector3f::UnitZ();
              if (
                distance <= 1.f &&
                boost::geometry::intersects(
                  segment,
                  lm::segment2f{
                    pos_ground_relative,
                    pos_ground_relative +
                      Eigen::Vector2f{projected[0], projected[2]}}))
              {
                  physics->rotate_character(
                    registry, entity, {0.f, lm::pi, 0.f});
              }
          }

          registry.replace<lmng::character_controller>(
            entity,
            lmng::character_controller{
              transform.rotation * Eigen::Vector3f::UnitZ()});
      });
}
