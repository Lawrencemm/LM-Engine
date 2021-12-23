#pragma once

#include <string>
#include <entt/entity/fwd.hpp>
#include <lmng/archetype.h>
#include <lmng/loaders/loader.h>

namespace lmng
{
class asset_cache;

class yaml_archetype_loader : public asset_loader_interface<archetype_data>
{
  public:
    std::shared_ptr<archetype_data>
      load(asset_cache &cache, const std::string &asset_path) override;
};

void load_archetype_data(
  const entt::entity &entity,
  entt::registry &registry,
  std::string const &asset_path,
  asset_cache &asset_cache);
}
