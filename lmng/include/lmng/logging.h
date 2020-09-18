#pragma once

#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

namespace lmng
{
void connect_component_logging(entt::registry &registry);
void connect_component_logging(
  entt::meta_type const &meta_type,
  entt::registry &registry);
std::string output_data_for_log(
  const entt::meta_any &component,
  const entt::registry &context);
} // namespace lmng
