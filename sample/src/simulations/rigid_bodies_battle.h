#pragma once
#include <entt/entt.hpp>

#include <lmengine/physics.h>
#include <lmtk/lmtk.h>

class rigid_bodies_battle
{
  public:
    explicit rigid_bodies_battle(entt::registry &registry);

    void handle_input_event(
      lmtk::input_event const &input_event,
      entt::registry &registry);
    lmng::physics physics;

    void update(
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state);

    void apply_movement_controls(
      entt::registry &registry,
      lmtk::input_state const &input_state,
      entt::entity protagonist) const;

    void move_enemies(entt::registry &registry, entt::entity protagonist) const;
};