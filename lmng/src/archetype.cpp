#include <entt/entt.hpp>
#include <lmng/archetype.h>
#include <lmng/transform.h>
#include <lmng/yaml_save_load.h>
#include <yaml-cpp/yaml.h>

namespace lmng
{
void load_archetype_data(
  const entt::entity &entity,
  entt::registry &registry,
  std::string const &asset_path,
  asset_cache &asset_cache)
{
    auto archetype_data = asset_cache.load<lmng::archetype_data>(asset_path);

    emplace_unset_components_from_yaml(
      registry, entity, archetype_data->yaml["components"]);

    create_children_from_yaml(
      registry, archetype_data->yaml["children"], asset_cache, entity);
}

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