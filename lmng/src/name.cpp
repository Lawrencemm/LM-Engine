#include <lmng/name.h>
#include "lmng/hierarchy.h"

namespace lmng
{
entt::entity
  find_entity(entt::registry const &registry, std::string const &entity_name)
{
    entt::entity found_entity{entt::null};

    for (auto [entity, name] : registry.view<lmng::name const>().proxy())
    {
        if (name.string == entity_name)
            found_entity = entity;
    }

    return found_entity;
}

std::string resolve_name(const entt::registry &registry, entt::entity entity)
{
    auto name = get_name(registry, entity);
    name.reserve(30);

    auto curr = entity;

    std::string parent_name_chain;
    while(auto maybe_parent = registry.try_get<parent>(curr))
    {
        auto parent = maybe_parent->entity;
        name.insert(0, get_name(registry, parent) + ".");
        curr = parent;
    }

    return name;
}
} // namespace lmng
