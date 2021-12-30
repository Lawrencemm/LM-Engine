#pragma once

#include <entt/entity/registry.hpp>

#include <lmng/animation.h>
#include <lmng/hierarchy.h>
#include <lmng/physics.h>
#include <lmng/simulation.h>
#include <lmng/transform.h>
#include <lmtk/event.h>
#include <yaml-cpp/node/node.h>

#include "../components/character_input.h"

class many_characters
{
  public:
    explicit many_characters(lmng::simulation_init const &init);

    void handle_input_event(
      lmtk::event const &input_event,
      entt::registry &registry);

    void update(
      entt::registry &registry,
      float dt,
      lmtk::input_state const &input_state);

    void spawn_robots(lmng::simulation_init const &init) const;

    lmng::physics physics;
    lmng::animation_system animation_system;
    lmng::hierarchy_system hierarchy_system;
    entt::entity camera;

    entt::entity ground;
    lmng::animation swing_arms_animation;
};
