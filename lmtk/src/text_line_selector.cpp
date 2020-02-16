#include <lmtk/text_line_selector.h>
#include <lmtk/vlayout.h>

static auto create_layouts(lmtk::text_line_selector_init const &init)
{
    std::vector<lmtk::text_layout> line_layouts;
    line_layouts.reserve(init.lines.size());
    for (auto &line_text : init.lines)
    {
        line_layouts.emplace_back(lmtk::text_layout_init{
          .renderer = *init.renderer,
          .material = init.font_material,
          .font = init.font,
          .colour = {1.f, 1.f, 1.f},
          .text = line_text,
        });
    }
    lmtk::layout_vertical(
      lmtk::vertical_layout{.start = 0, .spacing = 16}, line_layouts);
    return std::move(line_layouts);
}

namespace lmtk
{
text_line_selector::text_line_selector(text_line_selector_init const &init)
    : selection_index{0},
      line_layouts{create_layouts(init)},
      selection_background{
        *init.renderer,
        init.rect_material,
        line_layouts[selection_index].position,
        line_layouts[selection_index].get_size(),
        {0.f, 0.f, 0.f, 1.f},
      }
{
}

iwidget &text_line_selector::add_to_frame(lmgl::iframe *frame)
{
    selection_background.set_rect(
      line_layouts[selection_index].position,
      line_layouts[selection_index].get_size());
    selection_background.add_to_frame(frame);
    for (auto &layout : line_layouts)
    {
        layout.render(frame);
    }
    return *this;
}

lm::size2i text_line_selector::get_size()
{
    throw std::runtime_error{"Not implemented."};
}

lm::point2i text_line_selector::get_position()
{
    throw std::runtime_error{"Not implemented."};
}

iwidget &text_line_selector::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
}

iwidget &text_line_selector::move_resources(
  lmgl::irenderer *renderer,
  resource_sink &resource_sink)
{
    for (auto &layout : line_layouts)
        layout.move_resources(renderer, resource_sink);

    selection_background.move_resources(renderer, resource_sink);
    return *this;
}

bool text_line_selector::handle(lmtk::input_event const &input_event)
{
    return input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_event) {
                 switch (key_down_event.key)
                 {
                 case lmpl::key_code::I:
                     if (selection_index == 0)
                         return false;

                     selection_index -= 1;
                     return true;

                 case lmpl::key_code::K:
                     if (selection_index + 1 == line_layouts.size())
                         return false;

                     selection_index += 1;
                     return true;

                 default:
                     return false;
                 }
             },
             [](auto &) { return false; },
           };
}
} // namespace lmtk
