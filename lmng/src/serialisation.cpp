#include <range/v3/action/sort.hpp>
#include <yaml-cpp/yaml.h>

#include <lmng/name.h>
#include <lmng/reflection.h>
#include <lmng/serialisation.h>

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

void serialise(entt::registry &registry, YAML::Node &yaml)
{
    std::vector<entt::entity> entities;

    registry.each([&](entt::entity entity) {
        entities.emplace_back(entt::registry::entity(entity));
    });

    ranges::action::sort(entities);

    auto const type_map = lmng::create_meta_type_map();

    for (auto entity : entities)
    {
        YAML::Node actor_yaml;

        lmng::reflect_components(
          registry,
          entity,
          [&](lmng::any_component const &component_any) {
              actor_yaml[component_any.name()] =
                serialise_component(registry, component_any.any);
          },
          type_map);

        yaml[lmng::get_name(registry, entity)] = actor_yaml;
    };
}

void deserialise(YAML::Node const &yaml, entt::registry &registry)
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
