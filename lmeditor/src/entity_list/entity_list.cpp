#include "entity_list.h"

#include <entt/entt.hpp>

#include <lmengine/name.h>

namespace lmeditor
{
entity_list::entity_list(entity_list_init const &init)
    : view{init, *this}, selected_entity_index{0}, named_entities_count{0}
{
}

bool entity_list::handle(lmtk::input_event const &input_event)
{
    return input_event >> lm::variant_visitor{
                            [&](lmtk::key_down_event const &key_down_event) {
                                return handle_key_down(key_down_event);
                            },
                            [](auto) { return false; },
                          };
}

bool entity_list::handle_key_down(lmtk::key_down_event const &event)
{
    switch (event.key)
    {
    case lmpl::key_code::I:
        return move_selection(-1);

    case lmpl::key_code::K:
        return move_selection(1);

    default:
        return false;
    }
}

bool entity_list::move_selection(int movement)
{
    int new_pos = std::max(
      std::min(selected_entity_index + movement, named_entities_count - 1), 0);

    if (selected_entity_index == new_pos)
        return false;

    selected_entity_index = new_pos;

    return true;
}

lmtk::iwidget &entity_list::add_to_frame(lmgl::iframe *frame)
{
    view.update_selection_background(*this);
    view.add_to_frame(frame, *this);
    return *this;
}

lm::size2i entity_list::get_size() { return view.size; }

lm::point2i entity_list::get_position() { return view.position; }

lmtk::iwidget &entity_list::set_rect(lm::point2i position, lm::size2i size)
{
    view.position = position;
    view.size = size;
    return *this;
}

lmtk::iwidget &entity_list::move_resources(
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    view.move_resources(renderer, resource_sink);
    return *this;
}

std::vector<command_description> entity_list::get_command_descriptions()
{
    return std::vector<command_description>();
}

entity_list &entity_list::display(
  lmgl::irenderer &renderer,
  lmtk::resource_sink &resource_sink,
  lmtk::ifont const &font,
  entt::registry const &registry)
{
    view.display(renderer, resource_sink, font, registry, *this);
    selected_entity_index = 0;
    named_entities_count = view.line_layouts.size();
    return *this;
}

entt::entity entity_list::get_selection(entt::registry const &registry)
{
    return registry.view<lmng::name const>()[selected_entity_index];
}
} // namespace lmeditor
