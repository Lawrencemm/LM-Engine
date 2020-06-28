#pragma once

#include "asset_cache.h"
#include <filesystem>
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
} // namespace lmng

inline std::ostream &operator<<(std::ostream &os, lmng::archetype const &archetype)
{
    os << archetype.asset_path;
    return os;
}
