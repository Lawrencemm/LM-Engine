#pragma once
#include "asset_cache.h"
#include <entt/entt.hpp>
#include <filesystem>

namespace lmng
{
struct simulation_init
{
    entt::registry &registry;
    lmng::asset_cache &asset_cache;
};
} // namespace lmng
