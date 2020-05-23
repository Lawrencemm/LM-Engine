#include "inspector_component.h"
#include <entt/meta/meta.hpp>
#include <fmt/format.h>
#include <lmengine/name.h>
#include <lmlib/enumerate.h>
#include <lmtk/component.h>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>
#include <lmtk/vlayout.h>

namespace lmeditor
{

inspector inspector_init::operator()()
{
    return std::make_unique<inspector_component>(*this);
}

inspector_component::inspector_component(inspector_init const &init)
    : controller{init.registry},
      size{init.size},
      selection_background{
        init.renderer,
        init.resource_cache,
        {0, 0},
        {0, 0},
        std::array{0.f, 0.f, 0.f, 1.f},
      }
{
}

void inspector_component::display(
  entt::registry const &registry,
  entt::entity entity,
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache)
{
    clear(renderer, resource_sink);

    create_text(renderer, resource_cache);
    update_selection_background();
}

void inspector_component::create_text(
  lmgl::irenderer *renderer,
  lmtk::resource_cache const &resource_cache)
{
    int name_xpos = position.x + 5;

    auto maybe_edit_name =
      std::get_if<inspector_controller::edit_name_state>(&controller.state);

    auto name_text =
      maybe_edit_name ? maybe_edit_name->text_editor.text
                      : lmng::get_name(*controller.registry, controller.entity);

    lines.emplace_back(lmtk::text_layout{lmtk::text_layout_init{
      .renderer = *renderer,
      .resource_cache = resource_cache,
      .colour = {1.f, 1.f, 1.f},
      .position = {name_xpos, position.y},
      .text = "Name: " + name_text,
    }});

    auto data_indent_pix = 30;

    for (auto [i, entry] : ranges::views::enumerate(controller.entries))
    {
        auto meta_data = entry.meta_data;
        auto meta_type = entry.component_meta_type;

        if (meta_data)
        {
            auto maybe_edit_data =
              std::get_if<inspector_controller::edit_data_state>(
                &controller.state);

            auto data_text =
              maybe_edit_data && i == controller.selected_entry_index
                ? maybe_edit_data->text_editor.text
                : lmng::
                    any_component{*controller.registry, controller.entity, meta_type}
                      .get(meta_data, *controller.registry);

            lines.emplace_back(lmtk::text_layout{lmtk::text_layout_init{
              .renderer = *renderer,
              .resource_cache = resource_cache,
              .colour = {1.f, 1.f, 1.f},
              .position = {name_xpos + data_indent_pix, position.y},
              .text = fmt::format(
                "{}: {}", lmng::get_data_name(meta_data), data_text),
            }});
        }
        else if (meta_type)
        {
            lines.emplace_back(lmtk::text_layout{lmtk::text_layout_init{
              .renderer = *renderer,
              .resource_cache = resource_cache,
              .colour = {1.f, 1.f, 1.f},
              .position = {name_xpos, position.y},
              .text = lmng::get_type_name(meta_type),
            }});
        }
    }

    lmtk::layout_vertical(
      lmtk::vertical_layout{.start = position.y, .spacing = 15}, lines);
}

void inspector_component::clear(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    clear_text(renderer, resource_sink);
}

void inspector_component::clear_text(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    for (auto &line : lines)
        line.move_resources(resource_sink);

    lines.clear();
}

std::string inspector_component::format_component_data(
  std::string const &name,
  std::string const &repr)
{
    return fmt::format("{}: {}", name, repr);
}

lmtk::component_interface &
  inspector_component::move_resources(lmgl::resource_sink &sink)
{
    selection_background.move_resources(sink);
    for (auto &line : lines)
        line.move_resources(sink);
    return *this;
}

lm::size2i inspector_component::get_size() { return size; }

inspector_component &
  inspector_component::set_rect(lm::point2i position, lm::size2i size)
{
    this->position = position;
    this->size = size;
    return *this;
}

lm::point2i inspector_component::get_position() { return position; }

std::vector<command_description> inspector_component::get_command_descriptions()
{
    return std::vector<command_description>();
}

void inspector_component::update_selection_background()
{
    selection_background.set_rect(
      {position.x, lines[controller.selected_entry_index].position.y},
      {size.width, lines[controller.selected_entry_index].get_size().height});
}

component_interface &inspector_component::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache,
  lmtk::input_state const &input_state)
{
    clear(renderer, resource_sink);

    if (controller.entity != entt::null)
    {
        create_text(renderer, resource_cache);
        update_selection_background();
    }

    return *this;
}

bool inspector_component::add_to_frame(lmgl::iframe *frame)
{
    selection_background.add_to_frame(frame);
    for (auto &line : lines)
    {
        line.render(frame);
    }
    return false;
}

bool inspector_component::handle(const lmtk::input_event &input_event)
{
    return controller.handle(input_event);
}

void inspector_component::display(
  const entt::registry &registry,
  entt::entity entity)
{
    controller.inspect_entity(registry, entity);
}

void inspector_component::update(entt::registry &registry) {}

void inspector_component::clear() {}

} // namespace lmeditor
