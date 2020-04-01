#include <Eigen/Eigen>
#include <fmt/format.h>
#include <fstream>
#include <lmeditor/save_load_pose.h>
#include <lmengine/name.h>
#include <lmengine/transform.h>

static YAML::Node
  output_child(entt::registry const &registry, entt::entity child)
{
    YAML::Node yaml_node;

    auto transform = registry.get<lmng::transform>(child);
    YAML::Node transform_node;

    transform_node["Position"] = fmt::format(
      "{} {} {}",
      transform.position.x(),
      transform.position.y(),
      transform.position.z());
    transform_node["Rotation"] = fmt::format(
      "{} {} {} {}",
      transform.rotation.x(),
      transform.rotation.y(),
      transform.rotation.z(),
      transform.rotation.w());

    yaml_node["Transform"] = transform_node;

    return std::move(yaml_node);
}

namespace lmeditor
{
void load_pose(
  entt::registry &registry,
  entt::entity entity,
  YAML::Node const &pose_yaml)
{
    for (auto const &yaml_entity : pose_yaml)
    {
        auto child =
          lmng::find_entity(registry, yaml_entity.first.as<std::string>());

        Eigen::Vector3f pos;
        Eigen::Quaternionf rot;

        std::istringstream{
          yaml_entity.second["Transform"]["Position"].as<std::string>()} >>
          pos.x() >> pos.y() >> pos.z();
        std::istringstream{
          yaml_entity.second["Transform"]["Rotation"].as<std::string>()} >>
          rot.x() >> rot.y() >> rot.z() >> rot.w();

        registry.replace<lmng::transform>(child, lmng::transform{pos, rot});
    }
}

YAML::Node save_pose(entt::registry const &registry, entt::entity entity)
{
    YAML::Node yaml;

    lmng::visit_transform_children(registry, entity, [&](auto child) {
        yaml[lmng::get_name(registry, child)] = output_child(registry, child);
    });

    return yaml;
}
} // namespace lmeditor
