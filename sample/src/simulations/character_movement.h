#pragma once

#include <entt/entity/registry.hpp>

#include "../components/animation.h"
#include <lmengine/physics.h>
#include <lmengine/transform.h>
#include <lmtk/lmtk.h>

#include "../components/character_input.h"

class character_movement
{
  public:
    explicit character_movement(entt::registry &registry);

    void handle_input_event(
      lmtk::input_event const &input_event,
      entt::registry &registry);

    void update(
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state);

    lmng::physics physics;
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
    void camera_follow_character(
      entt::registry &registry,
      character const &character);

    static void
      apply_animation(character &character, entt::registry &registry, float dt);
};
