#include <lmng/name.h>

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
} // namespace lmng
