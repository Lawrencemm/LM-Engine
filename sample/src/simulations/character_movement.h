#pragma once

#include <entt/entity/registry.hpp>

#include <lmng/animation.h>
#include <lmng/hierarchy.h>
#include <lmng/physics.h>
#include <lmng/simulation.h>
#include <lmng/transform.h>
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
    lmng::hierarchy_system hierarchy_system;
    entt::entity camera;

    struct character
    {
        entt::entity entity;
        lmng::transform &transform;
        lmng::character_controller &controller;
    };

    static character get_player_character(entt::registry &registry);

    void apply_movement_controls(
      character &character,
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state);

    void move_robots(entt::registry &registry, float dt);

    void control_animation(entt::registry &registry, float dt);

    void camera_follow_character(
      entt::registry &registry,
      character const &character);

    entt::entity ground;
    lmng::animation swing_arms_animation;
};
