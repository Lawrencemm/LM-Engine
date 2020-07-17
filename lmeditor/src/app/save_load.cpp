#include "app.h"
#include <fstream>
#include <lmeditor/model/creation_time.h>
#include <lmeditor/model/selection.h>
#include <lmng/meta/clone.h>
#include <lmng/name.h>
#include <lmng/pose.h>
#include <lmng/yaml_save_load.h>
#include <yaml-cpp/yaml.h>

namespace lmeditor
{
bool editor_app::on_map_saved(const std::string &project_path)
{
    auto absolute = project_dir / (project_path + ".lmap");

    save_map(absolute, map);

    map_file_project_relative_path = project_path;

    change_state<gui_state>();

    lmng::connect_on_construct_any(map, *this);
    lmng::connect_on_replace_any(map, *this);
    lmng::connect_on_destroy_any(map, *this);

    return !visible_components.empty();
}

void editor_app::save_map(
  std::filesystem::path const &absolute_path,
  entt::registry &map_registry)
{
    map_registry.sort<lmng::name>([&](entt::entity lhs, entt::entity rhs) {
        return map_registry.get<creation_time>(lhs).time_point <
               map_registry.get<creation_time>(rhs).time_point;
    });

    auto yaml = YAML::Node{};
    lmng::save_registry_as_yaml(map_registry, asset_cache, yaml);

    std::ofstream output{absolute_path};
    output << yaml;
}

bool editor_app::on_map_selected(
  unsigned _unused_,
  const std::string &project_path)
{
    load_map(project_path);
    change_state<gui_state>();

    return true;
}

bool editor_app::load_map(std::filesystem::path const &path)
{
    lmng::disconnect_on_construct_any(map, *this);
    lmng::disconnect_on_replace_any(map, *this);
    lmng::disconnect_on_destroy_any(map, *this);

    auto map_yaml = YAML::LoadFile(path.string());
    map.clear();
    lmng::populate_registry_from_yaml(map_yaml, map, asset_cache);
    map_file_project_relative_path =
      std::filesystem::relative(path, project_dir).replace_extension().string();

    lmng::connect_on_construct_any(map, *this);
    lmng::connect_on_replace_any(map, *this);
    lmng::connect_on_destroy_any(map, *this);

    return true;
}

bool editor_app::on_pose_saved(const std::string &project_path)
{
    auto absolute = project_dir / (project_path + ".lpose");

    std::ofstream output{absolute};
    output << lmng::save_pose(map, get_selection(map));

    change_state<gui_state>();
    return true;
}

bool editor_app::on_pose_selected(
  unsigned int selection_index,
  std::string const &project_path)
{
    auto pose_path = project_dir / (project_path + ".lpose");

    lmng::load_pose(
      map, get_selection(map), YAML::LoadFile(pose_path.string()));

    change_state<gui_state>();

    return true;
}

void editor_app::save_map_async()
{
    auto serialise_registry = std::make_shared<entt::registry>();
    lmng::clone(*serialise_registry, map);

    auto p_age_entities = map.data<creation_time>();
    auto p_data = map.raw<creation_time>();

    serialise_registry->assign<creation_time>(
      p_age_entities, p_age_entities + map.size<creation_time>(), p_data);

    save_map_buffer_node.try_put(save_map_msg{std::move(serialise_registry)});
}
} // namespace lmeditor
