#pragma once

#include <entt/entity/registry.hpp>
#include <yaml-cpp/node/node.h>

namespace lmng
{
YAML::Node serialise_component(
  entt::registry const &registry,
  entt::meta_any const &component);

void serialise(entt::registry &registry, YAML::Node &yaml);
void deserialise(YAML::Node const &yaml, entt::registry &registry);
entt::registry deserialise(YAML::Node const &yaml);
} // namespace lmng
