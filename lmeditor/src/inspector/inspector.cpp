#include "inspector.h"

#include <entt/meta/meta.hpp>
#include <fmt/format.h>

#include <lmengine/name.h>
#include <lmengine/reflection.h>
#include <lmlib/enumerate.h>
#include <lmlib/variant_visitor.h>
#include <lmtk/text_editor.h>
#include <lmtk/text_layout.h>
#include <lmtk/vlayout.h>
#include <range/v3/algorithm/remove.hpp>
#include <range/v3/view/transform.hpp>

namespace lmeditor
{
pinspector create_inspector(
  lmgl::irenderer &renderer,
  lmgl::material text_material,
  lmtk::ifont const *font,
  lm::size2i const &size)
{
    return std::make_unique<inspector>(renderer, text_material, font, size);
}

inspector::inspector(
  lmgl::irenderer &renderer,
  lmgl::material text_material,
  lmtk::ifont const *font,
  lm::size2i const &size)
    : renderer{renderer},
      size{size},
      text_material{text_material},
      font{font},
      background_material{lmtk::rect::create_material(&renderer)},
      selection_background{
        renderer,
        background_material,
        {0, 0},
        {0, 0},
        std::array{0.f, 0.f, 0.f, 1.f},
      },
      state{empty_state{}},
      entity{entt::null}
{
}

void inspector::display(
  entt::registry const &registry,
  entt::entity entity,
  lmtk::resource_sink &resource_sink)
{
    clear(resource_sink);
    this->entity = entity;

    create_text(registry);
    state = select_state{};
    update_selection_background();
}

void inspector::create_text(const entt::registry &registry)
{
    int name_xpos = position.x + 5;

    labels.emplace_back(label{
      lmtk::text_layout{lmtk::text_layout_init{
        .renderer = renderer,
        .material = text_material,
        .font = font,
        .colour = {1.f, 1.f, 1.f},
        .position = {name_xpos, position.y},
        .text = "Name: ",
      }},
      false,
    });
    data_fields.emplace_back(lmtk::char_field_init{
      .renderer = renderer,
      .material = text_material,
      .font = font,
      .text_colour = {1.f, 1.f, 1.f},
      .position = {labels.back().text.get_size().width, 0},
      .initial = registry.get<lmng::name>(entity).string,
    });
    field_edit_handlers.emplace_back(
      edit_handler{[](auto value, auto &event_handler) {
          event_handler(inspector_changed_name{value});
      }});

    int component_label_xpos = name_xpos;
    lmng::reflect_components(
      registry, entity, [&](lmng::any_component const &component) {
          auto init = lmtk::text_layout_init{
            .renderer = renderer,
            .material = text_material,
            .font = font,
            .colour = {1.f, 1.f, 1.f},
            .position = {component_label_xpos, 0},
            .text = component.name(),
          };
          labels.emplace_back(label{
            lmtk::text_layout{init},
            false,
            0,
            component_meta_types.size(),
          });
          component_meta_types.emplace_back(component.any.type());

          int n_fields{0};
          int data_label_xpos = component_label_xpos + 30;
          component.any.type().data([&](entt::meta_data data) {
              auto text_repr = component.get(data, registry);
              auto field_title = lmng::get_data_name(data);

              auto data_label_init = lmtk::text_layout_init{
                .renderer = renderer,
                .material = text_material,
                .font = font,
                .colour = {1.f, 1.f, 1.f},
                .position = {data_label_xpos, 0},
                .text = std::string{field_title} + ": ",
              };
              labels.emplace_back(label{
                lmtk::text_layout{data_label_init}, true, data_fields.size()});

              int label_width = labels.back().text.get_size().width;

              data_fields.emplace_back(lmtk::char_field_init{
                .renderer = renderer,
                .material = text_material,
                .font = font,
                .text_colour = {1.f, 1.f, 1.f},
                .position = {data_label_xpos + label_width, 0},
                .initial = text_repr,
              });
              field_edit_handlers.emplace_back(
                edit_handler{[data](auto value, auto &event_handler) {
                    event_handler(inspector_updated_data{data, value});
                }});
              n_fields++;
          });
      });

    lmtk::layout_vertical(
      lmtk::vertical_layout{.start = position.y, .spacing = 15},
      ranges::views::transform(
        labels, [](auto &label) -> lmtk::text_layout & { return label.text; }));

    for (auto &label : labels)
    {
        if (!label.is_data)
            continue;

        auto &field = data_fields[label.data_index];
        auto position = label.text.position;
        position.x = field.get_position().x;
        field.set_position(position);
    }
}

void inspector::update(
  entt::registry &registry,
  lmtk::resource_sink &resource_sink)
{
    display(registry, entity, resource_sink);
}

void inspector::clear(lmtk::resource_sink &resource_sink)
{
    clear_text(resource_sink);

    field_edit_handlers.clear();
    component_meta_types.clear();

    selected_label_index = 0;
    state = empty_state{};
    entity = entt::null;
}

void inspector::clear_text(lmtk::resource_sink &resource_sink)
{
    for (auto &line : data_fields)
        line.move_resources(&renderer, resource_sink);

    for (auto &line : labels)
        line.text.move_resources(&renderer, resource_sink);

    data_fields.clear();
    labels.clear();
}

bool inspector::handle(
  lmtk::input_event const &event,
  lmtk::resource_sink &resource_sink,
  inspector_event_handler const &event_handler)
{
    return state >> lm::variant_visitor{[&](auto &state_alternative) {
               return state_alternative.handle(
                 {*this, event, resource_sink, event_handler});
           }};
}

inspector &inspector::add_to_frame(lmgl::iframe *frame, editor_app &app)
{
    render_base(frame);

    state >> lm::variant_visitor{
               [&](add_state &add_state) { add_state.render(frame); },
               [](auto) {},
             };
    return *this;
}

void inspector::render_base(lmgl::iframe *frame)
{
    selection_background.add_to_frame(frame);
    for (auto &field : data_fields)
    {
        field.add_to_frame(frame);
    }
    for (auto &label : labels)
    {
        label.text.render(frame);
    }
}

bool inspector::move_selection(int movement, lmtk::resource_sink &resource_sink)
{
    int new_pos = std::max(
      std::min(selected_label_index + movement, (int)labels.size() - 1), 0);
    if (selected_label_index == new_pos)
        return false;

    selected_label_index = new_pos;
    update_selection_background();
    return true;
}

std::string inspector::format_component_data(
  std::string const &name,
  std::string const &repr)
{
    return fmt::format("{}: {}", name, repr);
}

bool inspector::empty_state::handle(state_handle_args const &args)
{
    return false;
}

bool inspector::select_state::handle(state_handle_args const &args)
{
    return args.input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_event) {
                 switch (key_down_event.key)
                 {
                 case lmpl::key_code::I:
                 {
                     return args.inspector.move_selection(
                       -1, args.resource_sink);
                 }

                 case lmpl::key_code::K:
                 {
                     return args.inspector.move_selection(
                       1, args.resource_sink);
                 }

                 case lmpl::key_code::Enter:
                 {
                     if (
                       args.inspector.selected_label_index == 0 ||
                       args.inspector.selected_label().is_data)
                     {
                         args.inspector.state = edit_state{};
                         return true;
                     }
                     return false;
                 }

                 case lmpl::key_code::A:
                 {
                     args.inspector.state = args.inspector.create_add_state();
                     return true;
                 }

                 case lmpl::key_code::X:
                 {
                     if (args.inspector.selected_label().is_data)
                         return false;

                     args.event_handler(inspector_removed_component{
                       args.inspector.entity,
                       args.inspector.component_meta_types
                         [args.inspector.selected_label().component_index]});
                     return true;
                 }

                 default:
                     return false;
                 }
             },
             [&](auto) { return false; },
           };
}

bool inspector::edit_state::handle(state_handle_args const &args)
{
    return args.input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_msg) {
                 if (args.inspector.selected_field().handle(
                       key_down_msg,
                       &args.inspector.renderer,
                       args.inspector.font,
                       args.resource_sink))
                 {
                     return true;
                 }
                 switch (key_down_msg.key)
                 {
                 case lmpl::key_code::Enter:
                     args.inspector.save_selected_field(args.event_handler);
                     args.inspector.state = select_state{};
                     return true;

                 default:
                     return false;
                 }
             },
             [](auto) { return false; },
           };
}

lmtk::iwidget &
  inspector::move_resources(lmgl::irenderer *, lmtk::resource_sink &sink)
{
    sink.add(&renderer, background_material);
    selection_background.move_resources(&renderer, sink);
    state >> lm::variant_visitor{
               [&](add_state &add_state) {
                   add_state.move_resources(&renderer, sink);
               },
               [](auto) {},
             };
    return *this;
}

lm::size2i inspector::get_size() { return size; }

lmtk::iwidget &inspector::set_rect(lm::point2i position, lm::size2i size)
{
    this->position = position;
    this->size = size;
    return *this;
}

lm::point2i inspector::get_position() { return position; }

std::vector<command_description> inspector::get_command_descriptions()
{
    return std::vector<command_description>();
}

void inspector::update_selection_background()
{
    selection_background.set_rect(
      {position.x, selected_label_text().position.y},
      {size.width, selected_label_text().get_size().height});
}
} // namespace lmeditor
