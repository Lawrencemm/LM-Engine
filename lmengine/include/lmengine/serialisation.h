#pragma once

#include <entt/entity/registry.hpp>
#include <yaml-cpp/node/node.h>

namespace lmng
{
void serialise(entt::registry &registry, YAML::Node &yaml);
void deserialise(YAML::Node const &yaml, entt::registry &registry);
entt::registry deserialise(YAML::Node const &yaml);
} // namespace lmng
