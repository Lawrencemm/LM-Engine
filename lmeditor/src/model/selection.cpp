#include <entt/entt.hpp>
#include <lmeditor/model/selection.h>

namespace lmeditor
{
void select(entt::registry &registry, entt::entity entity)
{
    registry.clear<selected>();
    registry.emplace<selected>(entity);
}

entt::entity get_selection(const entt::registry &registry)
{
    entt::entity first_selected{entt::null};

    for (auto entity : registry.view<selected const>())
    {
        first_selected = entity;
        break;
    }

    return first_selected;
}
} // namespace lmeditor
