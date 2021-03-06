#pragma once

#include "asset_cache.h"
#include <entt/entt.hpp>
#include <filesystem>
#include <lmng/transform.h>
#include <string>
#include <yaml-cpp/yaml.h>

namespace lmng
{
struct archetype_data
{
    YAML::Node yaml;
};

struct archetype
{
    std::string asset_path;
};

void load_archetype_data(
  const entt::entity &entity,
  entt::registry &registry,
  std::string const &asset_path,
  asset_cache &asset_cache);

class yaml_archetype_loader : public asset_loader_interface<archetype_data>
{
  public:
    explicit yaml_archetype_loader(std::filesystem::path const &project_dir)
        : project_dir{project_dir}
    {
    }

    std::shared_ptr<archetype_data>
      load(asset_cache &cache, const std::string &asset_path) override
    {
        return std::make_shared<archetype_data>(archetype_data{YAML::LoadFile(
          (project_dir / "assets" / (asset_path + ".larch")).string())});
    }

  private:
    std::filesystem::path project_dir;
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
