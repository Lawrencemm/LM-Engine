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

void editor_app::toggle_component(size_t component_id)
{
    auto found_visible = ranges::find(visible_components, component_id);

    bool is_visible = found_visible != visible_components.end();
    bool is_focused = found_visible == visible_components.begin();

    if (is_visible)
        visible_components.erase(found_visible);

    if (!is_focused)
        visible_components.insert(visible_components.begin(), component_id);

    refit_visible_components();

    if (!visible_components.empty())
        update_active_component_border(get_current_component().get());
}

void editor_app::refit_visible_components()
{
    int total_width{0};
    auto window_size = resources.window->get_size_client();

    for (auto component_id : visible_components)
    {
        if (component_id == main_component_id)
            continue;

        total_width += components[component_id]->get_size().width;
    }

    int remainder = window_size.width - total_width;


    auto &main_component = components[main_component_id];
    auto main_component_size = main_component->get_size();
    main_component_size.width = remainder;
    main_component->set_rect({0, 0}, main_component_size);

    int current_pos{0};
    for (auto component_id : component_order)
    {
        if (
          ranges::find(visible_components, component_id) ==
          visible_components.end())
            continue;

        auto &component = components[component_id];

        auto component_size = component->get_size();
        component_size.height = window_size.height;

        component->set_rect({current_pos, 0}, component_size);
        current_pos += component_size.width;
    }

    update_active_component_border(get_current_component().get());
}
} // namespace lmeditor
