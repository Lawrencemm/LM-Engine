#include <range/v3/action/sort.hpp>
#include <yaml-cpp/yaml.h>

#include <lmengine/name.h>
#include <lmengine/reflection.h>
#include <lmengine/serialisation.h>

namespace lmng
{
void serialise(entt::registry &registry, YAML::Node &yaml)
{
    std::vector<std::tuple<entt::entity, meta_component const *>>
      sorted_entities;

    registry.view<meta_component>().each(
      [&](auto entity, meta_component const &meta_component) {
          sorted_entities.emplace_back(std::tuple{entity, &meta_component});
      });
    ranges::action::sort(sorted_entities, [](auto &entity, auto &other) {
        return std::get<0>(entity) < std::get<0>(other);
    });
    for (auto &[entity, meta_component] : sorted_entities)
    {
        YAML::Node actor_yaml;

        for (entt::meta_type const &component_type :
             meta_component->component_types)
        {
            YAML::Node component_yaml;

            entt::meta_any component =
              lmng::get_component_any(registry, entity, component_type);

            component_type.data([&](entt::meta_data data) {
                component_yaml[lmng::get_data_name(data)] =
                  lmng::get_data(component, data, registry);
            });
            actor_yaml[lmng::get_type_name(component_type)] = component_yaml;
        }
        yaml[lmng::get_name(registry, entity)] = actor_yaml;
    }
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

        meta_component meta_component;

        for (auto const &component_yaml : actor_yaml.second)
        {
            auto component_name = component_yaml.first.as<std::string>();
            auto component_meta_type =
              entt::resolve(entt::hashed_string{component_name.c_str()});
            auto component = component_meta_type.ctor().invoke();

            meta_component.component_types.push_back(component_meta_type);

            for (auto const &data_yaml : component_yaml.second)
            {
                auto data = component_meta_type.data(entt::hashed_string{
                  data_yaml.first.as<std::string>().c_str()});

                auto data_str = data_yaml.second.as<std::string>();
                lmng::set_data(component, data, data_str, registry);
            }
            lmng::assign_to_entity(component, registry, new_entity);
        }

        registry.assign<struct meta_component>(
          new_entity, std::move(meta_component));
    }
}

entt::registry deserialise(YAML::Node const &yaml)
{
    entt::registry registry;
    deserialise(yaml, registry);
    return std::move(registry);
}
} // namespace lmng
