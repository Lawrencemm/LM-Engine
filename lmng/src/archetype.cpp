#include <entt/entt.hpp>
#include <lmng/archetype.h>
#include <lmng/transform.h>
#include <yaml-cpp/yaml.h>
#include "lmng/loaders/archetype.h"

namespace lmng
{
entt::entity instantiate_archetype(
  entt::registry &registry,
  std::string const &archetype_asset_path,
  asset_cache &asset_cache,
  lmng::transform const &initial_transform)
{
    auto entity = registry.create();

    registry.emplace<lmng::transform>(entity, initial_transform);

    load_archetype_data(entity, registry, archetype_asset_path, asset_cache);

    return entity;
}
} // namespace lmng