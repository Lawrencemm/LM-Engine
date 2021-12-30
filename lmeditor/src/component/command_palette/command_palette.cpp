#include "lmeditor/component/command_palette.h"
#include "lmlib/variant_visitor.h"
#include <range/v3/action/push_back.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/transform.hpp>
#include <rapidfuzz/fuzz.hpp>

#include <lmtk/text_layout.h>
#include <lmtk/vlayout.h>

namespace lmeditor
{
lmtk::component command_palette_init::unique()
{
    return std::make_unique<command_palette>(*this);
}

std::array<lmtk::text_layout, 3>
  create_table_header(command_palette_init const &init)
{
    lmtk::text_layout_factory layout_factory{
      .renderer = init.renderer,
      .resource_cache = init.resource_cache,
      .colour = {1.f, 1.f, 1.f}};

    return std::array{
      layout_factory.create("Command"),
      layout_factory.create("Keystroke"),
      layout_factory.create("Context")};
}

std::vector<std::array<lmtk::text_layout, 3>>
  create_command_rows(command_palette_init const &init)
{
    std::vector<std::array<lmtk::text_layout, 3>> rows;

    lmtk::text_layout_factory layout_factory{
      .renderer = init.renderer,
      .resource_cache = init.resource_cache,
      .colour = {1.f, 1.f, 1.f}};

    for (auto const &command : init.commands)
    {
        rows.emplace_back(std::array{
          layout_factory.create(command.name),
          layout_factory.create(command.key),
          layout_factory.create(command.context)});
    }

    return std::move(rows);
}

command_palette::command_palette(command_palette_init const &init)
    : filter{lmtk::char_field_init{
        .renderer = init.renderer,
        .resource_cache = init.resource_cache,
        .text_colour = {1.f, 1.f, 1.f},
        .position = {0, 0},
        .initial = "",
      }},
      table_header{create_table_header(init)},
      rows{create_command_rows(init)},
      commands{init.commands}
{
    sorted_rows.reserve(rows.size());
}

lmtk::component_state command_palette::draw(lmtk::draw_event const &draw_event)
{
    auto filter_state = filter.handle(draw_event);

    auto filter_value = filter.get_value();

    if (filter_value.empty())
    {
        auto table_rows =
          ranges::views::concat(ranges::span{&table_header, 1}, rows);

        lmtk::table{table_rows, {0, filter.get_size().height}};

        for (auto &row : table_rows)
            for (auto &text_layout : row)
                text_layout.render(&draw_event.frame);

        return filter_state;
    }

    sorted_rows.clear();

    auto combine_with_score = [&](auto const &index_command_pair)
    {
         auto command_name_lower =
          ranges::views::transform(
            std::get<1>(index_command_pair).name, ::tolower) |
          ranges::to<std::string>;

        auto score = rapidfuzz::fuzz::ratio(filter_value, command_name_lower);

        return std::tuple(
          std::get<0>(index_command_pair),
          std::get<1>(index_command_pair),
          score);
    };

    auto get_index_and_score = [&](auto const &index_command_score)
    {
        return std::pair<size_t, double>{
          std::get<0>(index_command_score), std::get<2>(index_command_score)};
    };

    auto indices_with_scores = ranges::views::enumerate(commands) |
                    ranges::views::transform(combine_with_score) |
                    ranges::views::transform(get_index_and_score);

    ranges::push_back(sorted_rows, indices_with_scores);
    ranges::sort(
      sorted_rows, ranges::greater(), ranges::get<1, decltype(sorted_rows[0])>);

    auto sorted_to_layout_row =
      [&](auto const &index_score) -> decltype(rows[0])
    { return rows[std::get<0>(index_score)]; };

    auto filtered_layout_rows =
      sorted_rows | ranges::views::transform(sorted_to_layout_row);

    auto table_rows = ranges::views::concat(
      ranges::span{&table_header, 1}, filtered_layout_rows);

    lmtk::table{table_rows, {0, filter.get_size().height}};

    for (auto &row : table_rows)
        for (auto &text_layout : row)
            text_layout.render(&draw_event.frame);

    return filter_state;
}

lm::size2i command_palette::get_size()
{
    throw std::runtime_error{"Not implemented."};
}

lm::point2i command_palette::get_position()
{
    throw std::runtime_error{"Not implemented."};
}

command_palette &
  command_palette::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented."};
}

command_palette &
  command_palette::move_resources(lmgl::resource_sink &resource_sink)
{
    for (auto &layout : table_header)
        layout.move_resources(resource_sink);

    for (auto &row : rows)
        for (auto &layout : row)
            layout.move_resources(resource_sink);

    return *this;
}

lmtk::component_state command_palette::handle(lmtk::event const &event)
{
    return event >> lm::variant_visitor{
                      [&](lmtk::draw_event const &draw_event)
                      { return draw(draw_event); },
                      [&](auto const &event) { return filter.handle(event); }};
}

lm::point2i command_palette::get_table_origin()
{
    return {0, filter.get_size().height + 15};
}
} // namespace lmeditor
