#include <lmng/name.h>

namespace lmng
{
entt::entity
  find_entity(entt::registry const &registry, std::string const &entity_name)
{
    entt::entity found_entity{entt::null};

    registry.view<name const>().each([&](auto entity, auto &name_component) {
        if (name_component.string == entity_name)
            found_entity = entity;
    });

    return found_entity;
}
} // namespace lmng
