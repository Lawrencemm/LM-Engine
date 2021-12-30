#include <lmtk/char_field.h>
#include "lmlib/variant_visitor.h"

namespace lmtk
{

char_field::char_field(char_field_init const &init)
    : font{init.resource_cache.body_font.get()},
      layout{text_layout_init{
        .renderer = init.renderer,
        .resource_cache = init.resource_cache,
        .colour = init.text_colour,
        .position = init.position,
        .text = init.initial,
      }},
      editor{init.initial}
{
}

bool char_field::handle(event const &event)
{
    if (editor.handle(event))
    {
        dirty = true;
        return true;
    }
    event >> lm::variant_visitor{
               [&](lmtk::draw_event const &draw_event)
               {
                   if (dirty)
                   {
                       layout.set_text(
                         draw_event.renderer,
                         font,
                         editor.text,
                         draw_event.resource_sink);
                       dirty = false;
                   }
                   layout.render(&draw_event.frame);
               },
               [](auto) {},
             };
    return false;
}

lm::size2i char_field::get_size() { return layout.get_size(); }

lm::point2i char_field::get_position() { return layout.position; }

component_interface &char_field::set_rect(lm::point2i position, lm::size2i size)
{
    layout.set_position(position);
    return *this;
}

component_interface &
  char_field::move_resources(lmgl::resource_sink &resource_sink)
{
    layout.move_resources(resource_sink);
    return *this;
}
} // namespace lmtk
