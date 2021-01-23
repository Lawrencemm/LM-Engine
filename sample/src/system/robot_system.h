#pragma once

#include <entt/entity/registry.hpp>
#include <lmng/animation.h>
#include <lmng/physics.h>

void move_robots(entt::registry &registry, entt::entity ground);

void control_robots_animation(
  entt::registry &registry,
  lmng::physics &physics,
  lmng::animation_system &animation_system,
  lmng::animation const &animation);
