#pragma once

#include "asset_cache.h"
#include <entt/entity/registry.hpp>
#include <yaml-cpp/node/node.h>

namespace lmng
{
YAML::Node save_component_as_yaml(
  entt::registry const &registry,
  entt::meta_any const &component);

entt::meta_any construct_component_from_yaml(
  const entt::registry &registry,
  const YAML::Node &component_name_yaml,
  YAML::Node const &component_yaml);

entt::meta_any construct_component_from_yaml(
  const entt::registry &registry,
  std::string const &component_type_name,
  YAML::Node const &component_yaml);

void emplace_components_from_yaml(
  entt::registry &registry,
  entt::entity into_entity,
  YAML::Node const &components_yaml);

void emplace_unset_components_from_yaml(
  entt::registry &registry,
  entt::entity into_entity,
  YAML::Node const &components_yaml);

void create_children_from_yaml(
  entt::registry &registry,
  const YAML::Node &yaml,
  asset_cache &asset_cache,
  const entt::entity &entity);

void save_registry_as_yaml(
  entt::registry &registry,
  asset_cache &asset_cache,
  YAML::Node &yaml);
void populate_registry_from_yaml(
  YAML::Node const &yaml,
  entt::registry &registry,
  asset_cache &asset_cache);

entt::registry
  create_registry_from_yaml(YAML::Node const &yaml, asset_cache &asset_cache);
} // namespace lmng
