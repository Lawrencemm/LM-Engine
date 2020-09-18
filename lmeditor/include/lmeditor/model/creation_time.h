#pragma once

#include <chrono>
#include <entt/entt.hpp>

namespace lmeditor
{
struct creation_time
{
    std::chrono::steady_clock::time_point time_point;
};

inline void assign_creation_time(entt::registry &registry, entt::entity entity)
{
    registry.emplace<creation_time>(entity, std::chrono::steady_clock::now());
}
} // namespace lmeditor
