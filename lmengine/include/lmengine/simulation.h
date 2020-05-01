#pragma once
#include <entt/entt.hpp>
#include <filesystem>

namespace lmng
{
struct simulation_init
{
    entt::registry &registry;
    std::filesystem::path const &project_dir;
};
} // namespace lmng
