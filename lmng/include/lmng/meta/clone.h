#pragma once

#include <entt/entt.hpp>

namespace lmng
{
void clone(entt::registry &dst, entt::registry const &src);
inline entt::registry clone(entt::registry const &src)
{
    entt::registry new_registry;
    clone(new_registry, src);
    return new_registry;
}
} // namespace lmng
