#pragma once

#include "asset_cache.h"
#include <entt/entity/registry.hpp>
#include <yaml-cpp/node/node.h>

namespace lmng
{
YAML::Node serialise_component(
  entt::registry const &registry,
  entt::meta_any const &component);

entt::meta_any deserialise_component(
  const entt::registry &registry,
  const YAML::Node &component_name_yaml,
  YAML::Node const &component_yaml);

entt::meta_any deserialise_component(
  const entt::registry &registry,
  std::string const &component_type_name,
  YAML::Node const &component_yaml);

void serialise(
  entt::registry &registry,
  asset_cache &asset_cache,
  YAML::Node &yaml);
void deserialise(
  YAML::Node const &yaml,
  entt::registry &registry,
  asset_cache &asset_cache);

entt::registry deserialise(YAML::Node const &yaml, asset_cache &asset_cache);
} // namespace lmng
