#include <lmtk/char_field.h>
#include <chrono>
#include <spdlog/spdlog.h>
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
      cursor{
        init.renderer,
        init.resource_cache,
        init.position,
        lm::size2i{2, layout.get_size().height},
        std::array{1.f, 1.f, 1.f, 1.f},
      },
      editor{init.initial}
{
}

lmtk::component_state char_field::handle(event const &event)
{
    if (editor.handle(event))
    {
        dirty = true;
        return {0.f};
    }
    float seconds_to_next_blink = get_seconds_to_next_blink();
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

                   auto cursor_pos = get_position();
                   cursor_pos.x = layout.get_size().width;
                   cursor.set_rect(cursor_pos, cursor.get_size());

                   using namespace std::chrono;
                   auto now = system_clock::now();
                   if (ceil<seconds>(now) - now > milliseconds{500})
                       cursor.handle(draw_event);
               },
               [](auto) {},
             };
    return {seconds_to_next_blink};
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
    cursor.move_resources(resource_sink);
    return *this;
}

float char_field::get_seconds_to_next_blink()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto prev_second = floor<seconds>(now);

    auto sub_second = now - prev_second;

    if (sub_second < milliseconds{500})
        return duration_cast<duration<float>>(milliseconds{500} - sub_second)
          .count();

    return duration_cast<duration<float>>(milliseconds{1000} - sub_second)
      .count();
}
} // namespace lmtk
