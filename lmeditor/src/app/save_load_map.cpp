#include "app.h"

#include <lmengine/serialisation.h>
#include <yaml-cpp/yaml.h>

namespace lmeditor
{
void editor_app::save_map(std::filesystem::path const &absolute_path)
{
    auto yaml = YAML::Node{};
    lmng::serialise(map, yaml);

    std::ofstream output{absolute_path};
    output << yaml;
}

void editor_app::load_map(const std::string &project_path)
{
    auto map_yaml =
      YAML::LoadFile((project_dir / (project_path + ".lmap")).string());
    map.clear();
    lmng::deserialise(map_yaml, map);
    map_file_project_relative_path = project_path;
}
} // namespace lmeditor
