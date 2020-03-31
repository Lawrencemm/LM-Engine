#include "app.h"
#include <fmt/format.h>
#include <lmengine/name.h>
#include <lmengine/transform.h>
#include <yaml-cpp/yaml.h>

namespace lmeditor
{

YAML::Node output_child(entt::registry const &registry, entt::entity child)
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

void editor_app::save_pose(std::filesystem::path const &absolute_path)
{
    YAML::Node yaml;

    lmng::visit_transform_children(
      map, map_editor->get_selection(), [&](auto child) {
          yaml[lmng::get_name(map, child)] = output_child(map, child);
      });

    std::ofstream output{absolute_path};
    output << yaml;
}
} // namespace lmeditor
