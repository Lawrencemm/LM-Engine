#pragma once

#include <entt/entity/registry.hpp>

#include "../components/animation.h"
#include <lmengine/animation.h>
#include <lmengine/physics.h>
#include <lmengine/simulation.h>
#include <lmengine/transform.h>
#include <lmtk/input_event.h>
#include <yaml-cpp/node/node.h>

#include "../components/character_input.h"

class character_movement
{
  public:
    explicit character_movement(lmng::simulation_init const &init);

    void handle_input_event(
      lmtk::input_event const &input_event,
      entt::registry &registry);

    void update(
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state);

    lmng::physics physics;
    lmng::animation_system animation_system;
    entt::entity camera;

    struct character
    {
        entt::entity entity;
        lmng::transform &transform;
        lmng::character_controller &controller;
        character_skeleton &skeleton;
    };

    static character get_character(entt::registry &registry);

    void apply_movement_controls(
      character &character,
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state);
    void control_animation(
      character &character,
      entt::registry &registry,
      float dt,
      Eigen::Vector3f const &new_velocity);
    void camera_follow_character(
      entt::registry &registry,
      character const &character);

    lmng::pose left_forward_pose, right_forward_pose;
    lmng::animation swing_arms_animation;
};
