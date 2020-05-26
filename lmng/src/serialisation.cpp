#include <lmng/hierarchy.h>
#include <lmng/name.h>
#include <lmng/reflection.h>
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

YAML::Node serialise_entity(
  entt::registry const &registry,
  entt::entity entity,
  meta_type_map const &type_map)
{
    YAML::Node components_yaml;

    reflect_components(
      registry,
      entity,
      [&](lmng::any_component const &component_any) {
          if (component_any.any.type() == entt::resolve<parent>())
              return;

          components_yaml[component_any.name()] =
            serialise_component(registry, component_any.any);
      },
      type_map);

    std::vector<entt::entity> children;

    for (auto child : child_range{registry, entity})
    {
        children.emplace_back(child);
    }

    ranges::actions::sort(children);

    YAML::Node children_yaml;

    for (auto child : children)
    {
        children_yaml[get_name(registry, child)] =
          serialise_entity(registry, child, type_map);
    }

    YAML::Node entity_yaml;

    entity_yaml["components"] = components_yaml;
    entity_yaml["children"] = children_yaml;

    return entity_yaml;
}

void serialise(entt::registry &registry, YAML::Node &yaml)
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
          serialise_entity(registry, entity, type_map);
    };

    yaml["entities"] = entities_yaml;
    yaml["schema_version"] = "1";
}

void deserialise_entity(
  entt::registry &registry,
  std::pair<YAML::Node, YAML::Node> const &actor_yaml,
  entt::entity parent)
{
    auto new_entity = registry.create();

    auto name = actor_yaml.first.as<std::string>();

    registry.assign<lmng::name>(new_entity, name);

    if (parent != entt::null)
        registry.assign<lmng::parent>(new_entity, parent);

    for (auto const &component_yaml : actor_yaml.second["components"])
    {
        lmng::assign_to_entity(
          deserialise_component(
            registry, component_yaml.first, component_yaml.second),
          registry,
          new_entity);
    }

    for (auto const &child_yaml : actor_yaml.second["children"])
    {
        deserialise_entity(registry, child_yaml, new_entity);
    }
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

void deserialise(YAML::Node const &yaml, entt::registry &registry)
{
    auto version_node = yaml["schema_version"];

    if (!version_node.IsDefined())
    {
        deserialise_v0(yaml, registry);
        return;
    }

    for (auto const &entity_yaml : yaml["entities"])
    {
        deserialise_entity(registry, entity_yaml, entt::null);
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

entt::registry deserialise(YAML::Node const &yaml)
{
    entt::registry registry;
    deserialise(yaml, registry);
    return std::move(registry);
}
} // namespace lmng
