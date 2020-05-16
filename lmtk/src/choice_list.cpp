#include <lmlib/variant_visitor.h>
#include <lmtk/choice_list.h>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>
#include <lmtk/vlayout.h>

static auto create_layouts(lmtk::choice_list_init const &init)
{
    std::vector<lmtk::text_layout> line_layouts;
    line_layouts.reserve(init.choices.size());
    for (auto &line_text : init.choices)
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
choice_list choice_list_init::operator()() { return choice_list{*this}; }

lm::reference<choice_list> choice_list_init::unique()
{
    return std::make_unique<choice_list>(*this);
}

choice_list::choice_list(choice_list_init const &init)
    : selection_index{0},
      lines{init.choices},
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

widget_interface &choice_list::add_to_frame(lmgl::iframe *frame)
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

lm::size2i choice_list::get_size()
{
    throw std::runtime_error{"Not implemented."};
}

lm::point2i choice_list::get_position()
{
    throw std::runtime_error{"Not implemented."};
}

widget_interface &choice_list::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
}

widget_interface &choice_list::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    for (auto &layout : line_layouts)
        layout.move_resources(renderer, resource_sink);

    selection_background.move_resources(renderer, resource_sink);
    return *this;
}

component_interface &choice_list::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    return *this;
}

bool choice_list::handle(lmtk::input_event const &input_event)
{
    return input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_event) {
                 switch (key_down_event.key)
                 {
                 case lmpl::key_code::Enter:
                 {
                     bool is_someone_dirty{false};
                     selected.collect(
                       [&is_someone_dirty](bool handler_ret) {
                           is_someone_dirty = is_someone_dirty || handler_ret;
                       },
                       selection_index,
                       lines[selection_index]);
                     return is_someone_dirty;
                 }

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

entt::sink<bool(unsigned, std::string const &)> choice_list::on_selected()
{
    return selected;
}
} // namespace lmtk
