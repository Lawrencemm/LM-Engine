#include <lmtk/char_field.h>

namespace lmtk
{

char_field::char_field(char_field_init const &init)
    : font{init.font},
      layout{text_layout_init{
        .renderer = init.renderer,
        .material = init.material,
        .font = init.font,
        .colour = init.text_colour,
        .position = init.position,
        .text = init.initial,
      }},
      editor{init.initial}
{
}

bool char_field::handle(input_event const &event)
{
    if (editor.handle(event))
    {
        dirty = true;
        return true;
    }
    return false;
}

widget_interface &char_field::add_to_frame(lmgl::iframe *frame)
{
    layout.render(frame);
    return *this;
}

lm::size2i char_field::get_size() { return layout.get_size(); }

lm::point2i char_field::get_position() { return layout.position; }

widget_interface &char_field::set_rect(lm::point2i position, lm::size2i size)
{
    layout.set_position(position);
    return *this;
}

widget_interface &char_field::move_resources(lmgl::resource_sink &resource_sink)
{
    layout.move_resources(resource_sink);
    return *this;
}

component_interface &char_field::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    if (dirty)
    {
        layout.set_text(*renderer, font, editor.text, resource_sink);
        dirty = false;
    }
    return *this;
}
} // namespace lmtk
