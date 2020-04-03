#include <Eigen/Eigen>
#include <fmt/format.h>
#include <fstream>
#include <lmengine/name.h>
#include <lmengine/serialisation.h>
#include <lmengine/transform.h>
#include <yaml-cpp/yaml.h>

static YAML::Node
  output_child(entt::registry const &registry, entt::entity child)
{
    YAML::Node yaml_node;

    yaml_node["Transform"] =
      lmng::serialise_component(registry, registry.get<lmng::transform>(child));

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

        Eigen::Vector3f pos;
        Eigen::Quaternionf rot;

        std::istringstream{
          yaml_entity.second["Transform"]["Position"].as<std::string>()} >>
          pos.x() >> pos.y() >> pos.z();
        std::istringstream{
          yaml_entity.second["Transform"]["Rotation"].as<std::string>()} >>
          rot.x() >> rot.y() >> rot.z() >> rot.w();

        registry.replace<transform>(child, transform{pos, rot});
    }
}

YAML::Node save_pose(entt::registry const &registry, entt::entity entity)
{
    YAML::Node yaml;

    visit_transform_children(registry, entity, [&](auto child) {
        yaml[get_name(registry, child)] = output_child(registry, child);
    });

    return yaml;
}
} // namespace lmng