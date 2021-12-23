#pragma once

#include <entt/entt.hpp>
#include <lmng/transform.h>
#include <string>
#include <yaml-cpp/yaml.h>

namespace lmng
{
class asset_cache;

struct archetype_data
{
    YAML::Node yaml;
};

struct archetype
{
    std::string asset_path;
};

entt::entity instantiate_archetype(
  entt::registry &registry,
  std::string const &archetype_asset_path,
  asset_cache &asset_cache,
  lmng::transform const &initial_transform);
} // namespace lmng

inline std::ostream &
  operator<<(std::ostream &os, lmng::archetype const &archetype)
{
    os << archetype.asset_path;
    return os;
}
