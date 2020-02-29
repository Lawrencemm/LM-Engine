#include "character_movement.h"
#include <range/v3/algorithm/copy.hpp>

#include <iostream>
#include <lmengine/camera.h>
#include <lmengine/kinematic.h>
#include <lmengine/transform.h>
#include <lmlib/math_constants.h>

#include "../components/animation.h"
#include "../components/character_input.h"

character_movement::character
  character_movement::get_character(entt::registry &registry)
{
    auto character_controller_count =
      registry.size<lmng::character_controller>();

    if (character_controller_count != 1)
        throw std::runtime_error{
          "One and only one character controller is supported."};

    auto entity = registry.view<lmng::character_controller>()[0];
    return character{
      entity,
      registry.get<lmng::transform>(entity),
      registry.get<lmng::character_controller>(entity),
      registry.get<character_skeleton>(entity),
    };
}

character_movement::character_movement(entt::registry &registry)
    : physics{lmng::create_physics(registry)}, camera{registry.create()}
{
    registry.assign<lmng::transform>(camera);
    registry.assign<lmng::camera>(camera, 1.1f, 0.1f, 1000.f, true);
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
    auto character = get_character(registry);

    apply_movement_controls(character, registry, dt, input_state);

    physics->step(registry, dt);

    apply_animation(character, registry, dt);

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
            -M_PI * dt,
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
            M_PI * dt,
            0.f,
          });
    }
}

void character_movement::apply_animation(
  character_movement::character &character,
  entt::registry &registry,
  float dt)
{
    auto &left_transform =
      registry.get<lmng::transform>(character.skeleton.left_shoulder);
    auto &right_transform =
      registry.get<lmng::transform>(character.skeleton.right_shoulder);

    Eigen::Vector3f left_arm_forward_vector =
      left_transform.rotation * Eigen::Vector3f::UnitZ();

    float rotation = character.skeleton.arm_swing_speed * 2 * lm::pi * dt;
    float apex_angle = character.skeleton.arm_swing_height * lm::pi / 2;
    float curr_angle_left =
      std::asin(left_arm_forward_vector.dot(Eigen::Vector3f::UnitZ()));

    if (character.controller.requested_velocity.squaredNorm() > 0)
    {
        character.skeleton.state >>
          lm::variant_visitor{
            [&](character_skeleton::still) {
                character.skeleton.state.emplace<character_skeleton::swing>(
                  character_skeleton::swing{true});
            },
            [&](character_skeleton::swing &swing_state) {
                bool at_top = std::abs(curr_angle_left) >= apex_angle;

                if (at_top)
                {
                    left_transform.rotation = Eigen::AngleAxisf{
                      swing_state.left_forward ? lm::pi / 2 - apex_angle
                                               : lm::pi / 2 + apex_angle,
                      Eigen::Vector3f::UnitX()};
                    swing_state.left_forward = !swing_state.left_forward;
                }
            },
            [&](character_skeleton::relax const &relax_state) {
                character.skeleton.state.emplace<character_skeleton::swing>(
                  character_skeleton::swing{relax_state.left_forward});
            },
          };
    }
    else
    {
        character.skeleton.state >>
          lm::variant_visitor{
            [&](character_skeleton::still) {},
            [&](character_skeleton::swing const &swing_state) {
                character.skeleton.state.emplace<character_skeleton::relax>(
                  character_skeleton::relax{left_arm_forward_vector.dot(
                                              Eigen::Vector3f::UnitZ()) >= 0});
            },
            [&](character_skeleton::relax const &relax_state) {
                bool at_bottom = left_arm_forward_vector.dot(
                                   Eigen::Vector3f::UnitZ()) <= 0.f ==
                                 relax_state.left_forward;

                if (at_bottom)
                {
                    character.skeleton.state
                      .emplace<character_skeleton::still>();
                }
            },
          };
    }

    character.skeleton.state >>
      lm::variant_visitor{
        [&](character_skeleton::still) {},
        [&](character_skeleton::swing const &swing_state) {
            Eigen::AngleAxisf left_rotation{
              swing_state.left_forward ? -rotation : rotation,
              Eigen::Vector3f::UnitX(),
            };
            left_transform.rotation = left_rotation * left_transform.rotation;
            right_transform.rotation =
              left_rotation.inverse() * right_transform.rotation;
        },
        [&](character_skeleton::relax const &relax_state) {
            Eigen::AngleAxisf left_rotation{
              relax_state.left_forward ? rotation : -rotation,
              Eigen::Vector3f::UnitX(),
            };
            left_transform.rotation = left_rotation * left_transform.rotation;
            right_transform.rotation =
              left_rotation.inverse() * right_transform.rotation;
        },
      };
}
