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
          .resource_cache = init.resource_cache,
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
        init.resource_cache,
        line_layouts[selection_index].position,
        line_layouts[selection_index].get_size(),
        {0.f, 0.f, 0.f, 1.f},
      }
{
}

lm::size2i choice_list::get_size()
{
    throw std::runtime_error{"Not implemented."};
}

lm::point2i choice_list::get_position()
{
    throw std::runtime_error{"Not implemented."};
}

component_interface &
  choice_list::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
}

component_interface &
  choice_list::move_resources(lmgl::resource_sink &resource_sink)
{
    for (auto &layout : line_layouts)
        layout.move_resources(resource_sink);

    selection_background.move_resources(resource_sink);
    return *this;
}

lmtk::component_state choice_list::handle(lmtk::event const &event)
{
    return event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_event)
             {
                 switch (key_down_event.key)
                 {
                 case lmpl::key_code::Enter:
                 {
                     bool is_someone_dirty{false};
                     selected.collect(
                       [&is_someone_dirty](bool handler_ret)
                       { is_someone_dirty = is_someone_dirty || handler_ret; },
                       selection_index,
                       lines[selection_index]);
                     return is_someone_dirty ? lmtk::component_state{0.f}
                                             : lmtk::component_state{};
                 }

                 case lmpl::key_code::I:
                     if (selection_index == 0)
                         return lmtk::component_state{};

                     selection_index -= 1;
                     return lmtk::component_state{0.f};

                 case lmpl::key_code::K:
                     if (selection_index + 1 == line_layouts.size())
                         return lmtk::component_state{};

                     selection_index += 1;
                     return lmtk::component_state{0.f};

                 default:
                     return lmtk::component_state{};
                 }
             },
             [&](lmtk::draw_event const &draw_event)
             {
                 selection_background.set_rect(
                   line_layouts[selection_index].position,
                   line_layouts[selection_index].get_size());
                 selection_background.handle(draw_event);
                 for (auto &layout : line_layouts)
                 {
                     layout.render(&draw_event.frame);
                 }
                 return lmtk::component_state{};
             },
             [](auto &) { return lmtk::component_state{}; },
           };
}

entt::sink<bool(unsigned, std::string const &)> choice_list::on_selected()
{
    return selected;
}
} // namespace lmtk
