#include "app.h"
#include <range/v3/algorithm/find.hpp>

namespace lmeditor
{
void editor_app::update_active_component_border(
  lmtk::component_interface *component)
{
    active_component_border->set_rect(
      component->get_position(), component->get_size());
}

void editor_app::toggle_component(component_interface *pview)
{
    auto found_visible = ranges::find(visible_components, pview);

    bool is_visible = found_visible != visible_components.end();
    bool is_focused = found_visible == visible_components.begin();

    if (is_visible)
        visible_components.erase(found_visible);

    if (!is_focused)
        visible_components.insert(visible_components.begin(), pview);

    refit_visible_components();

    if (!visible_components.empty())
        update_active_component_border(visible_components.front());
}

void editor_app::refit_visible_components()
{
    int total_width{0};

    for (auto component : visible_components)
    {
        if (component == main_component)
            continue;

        total_width += component->get_size().width;
    }

    int remainder = resources.window_size.width - total_width;

    auto main_component_size = main_component->get_size();
    main_component_size.width = remainder;
    main_component->set_rect({0, 0}, main_component_size);

    int current_pos{0};
    for (auto component : component_order)
    {
        if (
          ranges::find(visible_components, component) ==
          visible_components.end())
            continue;

        auto component_size = component->get_size();

        component->set_rect({current_pos, 0}, component_size);
        current_pos += component_size.width;
    }

    update_active_component_border(visible_components.front());
}
} // namespace lmeditor
