#include <lmtk/char_field.h>

namespace lmtk
{

char_field::char_field(char_field_init const &init)
    : layout{text_layout_init{
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

bool char_field::handle(
  input_event const &event,
  lmgl::irenderer *renderer,
  ifont const *font,
  resource_sink &resource_sink)
{
    if (editor.handle(event))
    {
        layout.set_text(*renderer, font, editor.text, resource_sink);
        return true;
    }
    return false;
}

iwidget &char_field::add_to_frame(lmgl::iframe *frame)
{
    layout.render(frame);
    return *this;
}

lm::size2i char_field::get_size() { return layout.get_size(); }

lm::point2i char_field::get_position() { return layout.position; }

iwidget &char_field::set_rect(lm::point2i position, lm::size2i size)
{
    layout.set_position(position);
    return *this;
}

iwidget &char_field::move_resources(
  lmgl::irenderer *renderer,
  resource_sink &resource_sink)
{
    layout.move_resources(renderer, resource_sink);
    return *this;
}

char_field &char_field::set_position(lm::point2i new_position)
{
    layout.position = new_position;
    return *this;
}
} // namespace lmtk
