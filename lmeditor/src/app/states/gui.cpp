#include "../app.h"
#include <lmeditor/model/selection.h>
#include <lmlib/variant_visitor.h>
#include <lmng/name.h>
#include <lmtk/char_field.h>

namespace lmeditor
{
bool editor_app::gui_state::handle(
  editor_app &app,
  lmtk::input_event const &input_event)
{
    if (
      input_event >>
      lm::variant_visitor{
        [&](lmtk::key_down_event const &key_down_event) {
            if (key_down_event.input_state.key_state.alt())
            {
                if (
                  key_down_event.input_state.key_state.shift() &&
                  app.visible_components.front() != app.main_component_id)
                {
                    unsigned width_change{0};
                    switch (key_down_event.key)
                    {
                    case lmpl::key_code::L:
                        width_change = 5;
                        break;

                    case lmpl::key_code::J:
                        width_change = -5;
                        break;

                    default:
                        break;
                    }
                    if (width_change)
                    {
                        auto &current_component =
                          app.components[app.visible_components.front()];
                        auto new_size = current_component->get_size();
                        new_size.width += width_change;
                        current_component->set_rect(
                          current_component->get_position(), new_size);
                        app.refit_visible_components();
                        return true;
                    }
                }
                auto found_mapping =
                  app.key_code_view_map.find(key_down_event.key);
                if (found_mapping != app.key_code_view_map.end())
                {
                    app.toggle_component(found_mapping->second);
                    return true;
                }
            }

            switch (key_down_event.key)
            {
            case lmpl::key_code::F1:
                app.change_state(modal_state{app.create_command_help()});
                return true;

            case lmpl::key_code::P:
                if (key_down_event.input_state.key_state.control())
                {
                    app.change_state(
                      modal_state{app.create_simulation_selector()});
                    return true;
                }
                return false;
            case lmpl::key_code::R:
                if (key_down_event.input_state.key_state.control())
                {
                    app.change_state(modal_state{app.create_player()});
                    return true;
                }
                return false;

            case lmpl::key_code::S:
                if (key_down_event.input_state.key_state.control())
                {
                    app.change_state(modal_state{app.create_map_saver()});
                    return true;
                }
                return false;

            case lmpl::key_code::B:
                if (key_down_event.input_state.key_state.control())
                {
                    auto selected_entity = get_selection(app.map);

                    if (selected_entity == entt::null)
                        return false;

                    if (key_down_event.input_state.key_state.shift())
                    {
                        app.change_state(modal_state{app.create_pose_loader()});
                        return true;
                    }

                    app.change_state(modal_state{app.create_pose_saver(
                      lmng::get_name(app.map, selected_entity))});
                    return true;
                }
                return false;

            default:
                return false;
            }
        },
        [](auto) { return false; },
      })
        return true;

    return app.get_current_component()->handle(input_event, nullptr);
}

editor_app::gui_state::gui_state(editor_app &app) {}

bool editor_app::gui_state::add_to_frame(editor_app &app, lmgl::iframe *frame)
{
    for (auto panel_id : app.visible_components)
    {
        auto &ppanel = app.components[panel_id];
        std::any model = app.get_component_model(panel_id);
        ppanel->update(
          app.resources.renderer.get(),
          app.resources.resource_sink,
          app.resource_cache,
          app.resources.input_state,
          model);
        ppanel->add_to_frame(frame);
    }

    app.active_component_border->add_to_frame(frame);
    return false;
}

void editor_app::gui_state::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
}
} // namespace lmeditor
