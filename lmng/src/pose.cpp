#include <Eigen/Eigen>
#include <fmt/format.h>
#include <fstream>
#include <lmng/hierarchy.h>
#include <lmng/name.h>
#include <lmng/pose.h>
#include <lmng/serialisation.h>
#include <lmng/transform.h>
#include <yaml-cpp/yaml.h>

static YAML::Node
  output_child(entt::registry const &registry, entt::entity child)
{
    YAML::Node yaml_node;

    yaml_node["Transform"] =
      lmng::serialise_component(registry, registry.get<lmng::transform>(child));
    yaml_node["Children"] = lmng::save_pose(registry, child);

    return std::move(yaml_node);
}

namespace lmng
{
void load_pose(
  entt::registry &registry,
  entt::entity entity,
  YAML::Node const &pose_yaml)
{
    for (auto const &yaml_entity : pose_yaml)
    {
        auto child = find_entity(registry, yaml_entity.first.as<std::string>());

        registry.replace<transform>(
          child,
          deserialise_component(
            registry, "Transform", yaml_entity.second["Transform"])
            .cast<transform>());

        load_pose(registry, child, yaml_entity.second["Children"]);
    }
}

YAML::Node save_pose(entt::registry const &registry, entt::entity entity)
{
    YAML::Node yaml;

    for (auto child : child_range{registry, entity})
    {
        yaml[get_name(registry, child)] = output_child(registry, child);
    }

    return yaml;
}
} // namespace lmng