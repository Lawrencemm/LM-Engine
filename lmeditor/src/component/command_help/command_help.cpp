#include "lmeditor/component/command_help.h"
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/span.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/transform.hpp>

#include <lmtk/text_layout.h>
#include <lmtk/vlayout.h>

namespace lmeditor
{
lmtk::component command_help_init::unique()
{
    return std::make_unique<command_help>(*this);
}

std::vector<std::array<lmtk::text_layout, 3>>
  create_command_rows(command_help_init const &init)
{
    std::vector<std::array<lmtk::text_layout, 3>> rows;
    lmtk::text_layout_factory layout_factory{
      .renderer = init.renderer,
      .resource_cache = init.resource_cache,
      .colour = {1.f, 1.f, 1.f}};
    rows.emplace_back(std::array{
      layout_factory.create("Command"),
      layout_factory.create("Keystroke"),
      layout_factory.create("Context")});
    for (auto const &command : init.commands)
    {
        rows.emplace_back(std::array{
          layout_factory.create(command.name),
          layout_factory.create(command.key),
          layout_factory.create(command.context)});
    }
    return std::move(rows);
}

command_help::command_help(command_help_init const &init)
    : filter{lmtk::char_field_init{
        .renderer = init.renderer,
        .resource_cache = init.resource_cache,
        .text_colour = {1.f, 1.f, 1.f},
        .position = {0, 0},
        .initial = "",
      }},
      rows{create_command_rows(init)},
      commands{init.commands}
{
    lmtk::table{rows, get_table_origin()};
}

lmtk::component_interface &command_help::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache,
  lmtk::input_state const &input_state)
{
    return *this;
}

bool command_help::add_to_frame(lmgl::iframe *frame)
{
    filter.add_to_frame(frame);

    auto filter_value = filter.get_value();

    auto shown_rows = ranges::views::concat(
      ranges::span{&rows[0], 1},
      ranges::views::zip(commands, ranges::views::tail(rows)) |
        ranges::views::filter([&](auto const &desc_and_row) {
            return ranges::all_of(filter_value, [&](auto c) {
                auto lower = ranges::views::transform(
                  std::get<0>(desc_and_row).name, ::tolower);
                return ranges::find(lower, std::tolower(c)) != lower.end();
            });
        }) |
        ranges::views::transform([&](auto const &pair) -> decltype(rows[0]) {
            return std::get<1>(pair);
        }));

    lmtk::table{shown_rows, {0, filter.get_size().height}};

    for (auto &row : shown_rows)
        for (auto &text_layout : row)
            text_layout.render(frame);
    return false;
}

lm::size2i command_help::get_size()
{
    throw std::runtime_error{"Not implemented."};
}

lm::point2i command_help::get_position()
{
    throw std::runtime_error{"Not implemented."};
}

command_help &command_help::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
}

command_help &command_help::move_resources(lmgl::resource_sink &resource_sink)
{
    for (auto &row : rows)
        for (auto &layout : row)
            layout.move_resources(resource_sink);

    return *this;
}

bool command_help::handle(lmtk::input_event const &input_event)
{
    return filter.handle(input_event);
}

lm::point2i command_help::get_table_origin()
{
    return {0, filter.get_size().height + 15};
}
} // namespace lmeditor
