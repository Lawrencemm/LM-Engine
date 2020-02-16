#include "model.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <lmengine/serialisation.h>

namespace lmeditor
{
void map_editor_model::load_map(std::filesystem::path const &file_path)
{
    auto map_yaml = YAML::LoadFile(file_path.string());
    map.reset();
    lmng::deserialise(map_yaml, map);
}
void map_editor_model::save_map(std::filesystem::path const &file_path)
{
    auto yaml = YAML::Node{};
    lmng::serialise(map, yaml);

    std::ofstream output{file_path};
    output << yaml;
}
} // namespace lmeditor
