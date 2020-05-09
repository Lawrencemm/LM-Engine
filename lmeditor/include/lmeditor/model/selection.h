#pragma once

#include <entt/fwd.hpp>

namespace lmeditor
{
struct selected
{
};

void select(entt::registry &registry, entt::entity entity);
entt::entity get_selection(entt::registry const &registry);
} // namespace lmeditor
