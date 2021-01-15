#include <lmng/archetype.h>
#include <lmng/asset_cache.h>
#include <lmng/hierarchy.h>
#include <lmng/meta/any_component.h>
#include <lmng/name.h>
#include <lmng/transform.h>
#include <lmng/yaml_save_load.h>
#include <range/v3/action/sort.hpp>
#include <yaml-cpp/yaml.h>

namespace lmng
{
YAML::Node save_component_as_yaml(
  entt::registry const &registry,
  const entt::meta_any &component)
{
    YAML::Node component_yaml;

    for (auto data : component.type().data())
    {
        component_yaml[lmng::get_data_name(data)] =
          get_data(component, data, registry);
    };

    return component_yaml;
}

std::vector<entt::entity>
  get_sorted_children(entt::registry const &registry, entt::entity parent)
{
    std::vector<entt::entity> children;

    for (auto child : child_range{registry, parent})
    {
        children.emplace_back(child);
    }

    ranges::actions::sort(children);

    return std::move(children);
}

void save_component_for_archetypal_entity(
  YAML::Node &components_yaml,
  YAML::Node const &archetype_yaml,
  entt::registry const &registry,
  any_component const &component_any)
{
    if (
      component_any.any.type() == entt::resolve<parent>() ||
      component_any.any.type() == entt::resolve<lmng::archetype>() ||
      component_any.any.type() == entt::resolve<name>())
        return;

    auto component_yaml = save_component_as_yaml(registry, component_any.any);

    auto archetype_components_yaml = archetype_yaml["components"];

    auto archetype_component_yaml =
      archetype_components_yaml[component_any.name()];

    if (!archetype_component_yaml)
    {
        // Archetype doesn't have the component, save it
        components_yaml[component_any.name()] = component_yaml;
    }
    else
    {
        bool is_archetype_component_data_different{false};
        for (auto const &archetype_field : archetype_component_yaml)
        {
            if (
              component_yaml[archetype_field.first.as<std::string>()]
                .as<std::string>() != archetype_field.second.as<std::string>())
            {
                is_archetype_component_data_different = true;
                break;
            }
        }

        if (is_archetype_component_data_different)
        {
            components_yaml[component_any.name()] = component_yaml;
        }
    }
}

YAML::Node save_archetypal_entity_as_yaml(
  entt::registry const &registry,
  entt::entity entity,
  meta_type_map const &type_map,
  archetype const &archetype,
  asset_cache &asset_cache)
{
    auto archetype_data =
      asset_cache.load<lmng::archetype_data>(archetype.asset_path);
    auto &archetype_yaml = archetype_data->yaml;

    YAML::Node components_yaml;

    visit_components(
      registry,
      entity,
      [&](lmng::any_component const &component_any) {
          save_component_for_archetypal_entity(
            components_yaml, archetype_yaml, registry, component_any);
      },
      type_map);

    YAML::Node entity_yaml;

    entity_yaml["archetype"] = archetype.asset_path;
    entity_yaml["components"] = components_yaml;
    entity_yaml["children"] = YAML::Node{};

    return entity_yaml;
}

YAML::Node save_entity_as_yaml(
  entt::registry const &registry,
  entt::entity entity,
  meta_type_map const &type_map,
  asset_cache &asset_cache)
{
    if (auto parchetype = registry.try_get<archetype>(entity))
    {
        return save_archetypal_entity_as_yaml(
          registry, entity, type_map, *parchetype, asset_cache);
    }

    YAML::Node components_yaml;

    visit_components(
      registry,
      entity,
      [&](lmng::any_component const &component_any) {
          if (
            component_any.any.type() == entt::resolve<parent>() ||
            component_any.any.type() == entt::resolve<name>())
              return;

          components_yaml[component_any.name()] =
            save_component_as_yaml(registry, component_any.any);
      },
      type_map);

    auto children = get_sorted_children(registry, entity);

    YAML::Node children_yaml;

    for (auto child : children)
    {
        children_yaml[get_name(registry, child)] =
          save_entity_as_yaml(registry, child, type_map, asset_cache);
    }

    YAML::Node entity_yaml;

    entity_yaml["components"] = components_yaml;
    entity_yaml["children"] = children_yaml;

    return entity_yaml;
}

void save_registry_as_yaml(
  entt::registry &registry,
  asset_cache &asset_cache,
  YAML::Node &yaml)
{
    auto const type_map = lmng::create_meta_type_map();

    YAML::Node entities_yaml;

    for (auto entity : registry.view<name>(entt::exclude<parent>))
    {
        entities_yaml[get_name(registry, entity)] =
          save_entity_as_yaml(registry, entity, type_map, asset_cache);
    };

    yaml["entities"] = entities_yaml;
    yaml["schema_version"] = "1";
}

void emplace_components_from_yaml(
  entt::registry &registry,
  entt::entity into_entity,
  YAML::Node const &components_yaml)
{
    for (auto const &component_yaml : components_yaml)
    {
        emplace_on_entity(
          construct_component_from_yaml(
            registry, component_yaml.first, component_yaml.second),
          registry,
          into_entity);
    }
}

void emplace_or_replace_components_from_yaml(
  entt::registry &registry,
  entt::entity into_entity,
  YAML::Node const &components_yaml)
{
    for (auto const &component_yaml : components_yaml)
    {
        emplace_or_replace_on_entity(
          construct_component_from_yaml(
            registry, component_yaml.first, component_yaml.second),
          registry,
          into_entity);
    }
}

void create_entity_from_yaml(
  entt::registry &registry,
  std::string const &name,
  YAML::Node const &yaml,
  asset_cache &asset_cache,
  entt::entity parent);

void create_children_from_yaml(
  entt::registry &registry,
  const YAML::Node &yaml,
  asset_cache &asset_cache,
  const entt::entity &entity)
{
    for (auto const &child_yaml : yaml)
    {
        create_entity_from_yaml(
          registry,
          child_yaml.first.as<std::string>(),
          child_yaml.second,
          asset_cache,
          entity);
    }
}

void create_entity_from_yaml(
  entt::registry &registry,
  std::string const &name,
  YAML::Node const &yaml,
  asset_cache &asset_cache,
  entt::entity parent)
{
    auto entity = registry.create();

    registry.emplace<lmng::name>(entity, name);

    if (parent != entt::null)
    {
        registry.emplace<lmng::parent>(entity, parent);
    }

    auto archetype_yaml = yaml["archetype"];

    if (archetype_yaml)
    {
        auto asset_path = archetype_yaml.as<std::string>();
        load_archetype_data(entity, registry, asset_path, asset_cache);
        registry.emplace<archetype>(entity, asset_path);
        emplace_or_replace_components_from_yaml(
          registry, entity, yaml["components"]);
    }
    else
    {
        emplace_components_from_yaml(registry, entity, yaml["components"]);
    }

    create_children_from_yaml(registry, yaml["children"], asset_cache, entity);
}

void populate_registry_from_yaml(
  YAML::Node const &yaml,
  entt::registry &registry,
  asset_cache &asset_cache)
{
    for (auto const &entity_yaml : yaml["entities"])
    {
        create_entity_from_yaml(
          registry,
          entity_yaml.first.as<std::string>(),
          entity_yaml.second,
          asset_cache,
          entt::null);
    }
}

entt::meta_any construct_component_from_yaml(
  entt::registry const &registry,
  std::string const &component_type_name,
  YAML::Node const &component_yaml)
{
    auto component_meta_type =
      entt::resolve_id(entt::hashed_string{component_type_name.c_str()});

    auto component = component_meta_type.ctor<>().invoke();

    for (auto const &data_yaml : component_yaml)
    {
        auto data = component_meta_type.data(
          entt::hashed_string{data_yaml.first.as<std::string>().c_str()});

        auto data_str = data_yaml.second.as<std::string>();
        set_data(component, data, data_str, registry);
    }

    return component;
}

entt::meta_any construct_component_from_yaml(
  const entt::registry &registry,
  const YAML::Node &component_name_yaml,
  YAML::Node const &component_yaml)
{
    return construct_component_from_yaml(
      registry, component_name_yaml.as<std::string>(), component_yaml);
}

entt::registry
  create_registry_from_yaml(YAML::Node const &yaml, asset_cache &asset_cache)
{
    entt::registry registry;
    populate_registry_from_yaml(yaml, registry, asset_cache);
    return std::move(registry);
}
} // namespace lmng
