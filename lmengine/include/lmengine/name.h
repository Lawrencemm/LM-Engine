#pragma once

#include <string>

#include <entt/entt.hpp>

#include "named_type.h"

namespace lmng
{
struct name
{
    std::string string;
};

inline std::string get_name(entt::registry const &registry, entt::entity entity)
{
    auto name_component = registry.try_get<name>(entity);

    return name_component ? name_component->string
                          : std::to_string(to_integer(entity));
}

entt::entity
  find_entity(entt::registry const &registry, std::string const &name);
} // namespace lmng

LMNG_NAMED_TYPE(lmng::name, "Name");
