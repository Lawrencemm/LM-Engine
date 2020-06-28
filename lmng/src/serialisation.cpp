#include <lmng/archetype.h>
#include <lmng/asset_cache.h>
#include <lmng/hierarchy.h>
#include <lmng/meta/any_component.h>
#include <lmng/name.h>
#include <lmng/serialisation.h>
#include <lmng/transform.h>
#include <range/v3/action/sort.hpp>
#include <yaml-cpp/yaml.h>

namespace lmng
{
YAML::Node serialise_component(
  entt::registry const &registry,
  const entt::meta_any &component)
{
    YAML::Node component_yaml;

    component.type().data([&](entt::meta_data data) {
        component_yaml[lmng::get_data_name(data)] =
          get_data(component, data, registry);
    });

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

YAML::Node serialise_archetypal_entity(
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
          if (
            component_any.any.type() == entt::resolve<parent>() ||
            component_any.any.type() == entt::resolve<lmng::archetype>())
              return;

          auto component_yaml =
            serialise_component(registry, component_any.any);

          auto archetype_components_yaml = archetype_yaml["components"];

          if (
            auto archetype_component_yaml =
              archetype_components_yaml[component_any.name()])
          {
              bool different{false};
              for (auto const &archetype_field : archetype_component_yaml)
              {
                  if (
                    component_yaml[archetype_field.first.as<std::string>()]
                      .as<std::string>() !=
                    archetype_field.second.as<std::string>())
                  {
                      different = true;
                      break;
                  }
              }

              if (different)
              {
                  components_yaml[component_any.name()] = component_yaml;
              }
          }
      },
      type_map);

    YAML::Node entity_yaml;

    entity_yaml["archetype"] = archetype.asset_path;
    entity_yaml["components"] = components_yaml;
    entity_yaml["children"] = YAML::Node{};

    return entity_yaml;
}

YAML::Node serialise_entity(
  entt::registry const &registry,
  entt::entity entity,
  meta_type_map const &type_map,
  asset_cache &asset_cache)
{
    if (auto parchetype = registry.try_get<archetype>(entity))
    {
        return serialise_archetypal_entity(
          registry, entity, type_map, *parchetype, asset_cache);
    }

    YAML::Node components_yaml;

    visit_components(
      registry,
      entity,
      [&](lmng::any_component const &component_any) {
          if (component_any.any.type() == entt::resolve<parent>())
              return;

          components_yaml[component_any.name()] =
            serialise_component(registry, component_any.any);
      },
      type_map);

    auto children = get_sorted_children(registry, entity);

    YAML::Node children_yaml;

    for (auto child : children)
    {
        children_yaml[get_name(registry, child)] =
          serialise_entity(registry, child, type_map, asset_cache);
    }

    YAML::Node entity_yaml;

    entity_yaml["components"] = components_yaml;
    entity_yaml["children"] = children_yaml;

    return entity_yaml;
}

void serialise(
  entt::registry &registry,
  asset_cache &asset_cache,
  YAML::Node &yaml)
{
    std::vector<entt::entity> entities;

    registry.view<name>(entt::exclude<parent>)
      .each([&](entt::entity entity, auto &name) {
          entities.emplace_back(entity);
      });

    ranges::action::sort(entities);

    auto const type_map = lmng::create_meta_type_map();

    YAML::Node entities_yaml;

    for (auto entity : entities)
    {
        entities_yaml[get_name(registry, entity)] =
          serialise_entity(registry, entity, type_map, asset_cache);
    };

    yaml["entities"] = entities_yaml;
    yaml["schema_version"] = "1";
}

void deserialise_components(
  entt::registry &registry,
  entt::entity into_entity,
  YAML::Node const &components_yaml)
{
    for (auto const &component_yaml : components_yaml)
    {
        assign_to_entity(
          deserialise_component(
            registry, component_yaml.first, component_yaml.second),
          registry,
          into_entity);
    }
}

void deserialise_entity(
  entt::registry &registry,
  std::string const &name,
  YAML::Node const &yaml,
  asset_cache &asset_cache,
  entt::entity parent);

void deserialise_children(
  entt::registry &registry,
  const YAML::Node &yaml,
  asset_cache &asset_cache,
  const entt::entity &entity)
{
    for (auto const &child_yaml : yaml)
    {
        deserialise_entity(
          registry,
          child_yaml.first.as<std::string>(),
          child_yaml.second,
          asset_cache,
          entity);
    }
}

void deserialise_archetype(
  const entt::entity &entity,
  entt::registry &registry,
  std::string const &asset_path,
  asset_cache &asset_cache)
{
    auto archetype_data = asset_cache.load<lmng::archetype_data>(asset_path);

    deserialise_components(
      registry, entity, archetype_data->yaml["components"]);

    deserialise_children(
      registry, archetype_data->yaml["children"], asset_cache, entity);
}

void deserialise_entity(
  entt::registry &registry,
  std::string const &name,
  YAML::Node const &yaml,
  asset_cache &asset_cache,
  entt::entity parent)
{
    auto entity = registry.create();

    registry.assign<lmng::name>(entity, name);

    if (parent != entt::null)
    {
        registry.assign<lmng::parent>(entity, parent);
    }

    auto archetype_yaml = yaml["archetype"];

    if (archetype_yaml)
    {
        auto asset_path = archetype_yaml.as<std::string>();
        deserialise_archetype(entity, registry, asset_path, asset_cache);
        registry.assign<archetype>(entity, asset_path);
    }

    deserialise_components(registry, entity, yaml["components"]);
    deserialise_children(registry, yaml["children"], asset_cache, entity);
}

void deserialise_v0(YAML::Node const &yaml, entt::registry &registry)
{
    std::unordered_map<std::string, entt::entity> name_map;

    for (auto const &actor_yaml : yaml)
    {
        auto new_entity = registry.create();

        auto name = actor_yaml.first.as<std::string>();

        registry.assign<lmng::name>(new_entity, name);

        name_map.emplace(name, new_entity);
    }

    for (auto const &actor_yaml : yaml)
    {
        auto new_entity = name_map[actor_yaml.first.as<std::string>()];

        for (auto const &component_yaml : actor_yaml.second)
        {
            lmng::assign_to_entity(
              deserialise_component(
                registry, component_yaml.first, component_yaml.second),
              registry,
              new_entity);
        }
    }
}

void deserialise(
  YAML::Node const &yaml,
  entt::registry &registry,
  asset_cache &asset_cache)
{
    auto version_node = yaml["schema_version"];

    if (!version_node.IsDefined())
    {
        deserialise_v0(yaml, registry);
        return;
    }

    for (auto const &entity_yaml : yaml["entities"])
    {
        deserialise_entity(
          registry,
          entity_yaml.first.as<std::string>(),
          entity_yaml.second,
          asset_cache,
          entt::null);
    }
}

entt::meta_any deserialise_component(
  const entt::registry &registry,
  std::string const &component_type_name,
  YAML::Node const &component_yaml)
{
    auto component_meta_type =
      entt::resolve(entt::hashed_string{component_type_name.c_str()});

    auto component = component_meta_type.ctor().invoke();

    for (auto const &data_yaml : component_yaml)
    {
        auto data = component_meta_type.data(
          entt::hashed_string{data_yaml.first.as<std::string>().c_str()});

        auto data_str = data_yaml.second.as<std::string>();
        set_data(component, data, data_str, registry);
    }

    return component;
}

entt::meta_any deserialise_component(
  const entt::registry &registry,
  const YAML::Node &component_name_yaml,
  YAML::Node const &component_yaml)
{
    return deserialise_component(
      registry, component_name_yaml.as<std::string>(), component_yaml);
}

entt::registry deserialise(YAML::Node const &yaml, asset_cache &asset_cache)
{
    entt::registry registry;
    deserialise(yaml, registry, asset_cache);
    return std::move(registry);
}
} // namespace lmng
