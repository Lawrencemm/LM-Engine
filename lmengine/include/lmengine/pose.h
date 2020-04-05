#include <entt/fwd.hpp>
#include <yaml-cpp/yaml.h>

namespace lmng
{
void load_pose(
  entt::registry &registry,
  entt::entity entity,
  YAML::Node const &pose_yaml);

YAML::Node save_pose(entt::registry const &registry, entt::entity entity);
} // namespace lmng
