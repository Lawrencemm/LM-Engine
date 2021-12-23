#include <lmng/loaders/archetype.h>
#include <lmng/yaml_save_load.h>

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

std::shared_ptr<archetype_data> lmng::yaml_archetype_loader::load(
  asset_cache &cache,
  const std::string &asset_path)
{
    return std::make_shared<archetype_data>(archetype_data{YAML::LoadFile(
      (cache.content_dir / (asset_path + ".larch")).string())});
}
}
