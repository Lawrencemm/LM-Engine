#include "../app.h"
#include "../map_saver.h"
#include <lmeditor/map_selector.h>
#include <lmtk/char_field.h>

namespace lmeditor
{
bool gui_state::handle_input_event(state_handle_args const &args)
{
    if (
      args.input_event >>
      lm::variant_visitor{
        [&](lmtk::key_down_event const &key_down_event) {
            switch (key_down_event.key)
            {
            case lmpl::key_code::I:
                if (key_down_event.input_state.key_state.alt())
                {
                    args.app.toggle_tool_panel(args.app.inspector.get());
                    return true;
                }
                return false;

            case lmpl::key_code::M:
                if (key_down_event.input_state.key_state.alt())
                {
                    args.app.toggle_tool_panel(args.app.map_editor.get());
                    return true;
                }
                return false;

            case lmpl::key_code::F1:
                args.app.state = modal_state{
                  std::make_unique<command_help>(command_help_init{
                    .renderer = *args.app.resources.renderer,
                    .material = args.app.resources.text_material,
                    .font = args.app.resources.font.get(),
                    .commands = args.app.visible_panels.front()
                                  ->get_command_descriptions(),
                  }),
                  [](editor_app &app, auto help_widget, auto &input_event) {
                      return dynamic_cast<command_help *>(help_widget)
                        ->handle(
                          input_event,
                          app.resources.renderer.get(),
                          app.resources.font.get(),
                          app.resources.resource_sink);
                  }};
                return true;

            case lmpl::key_code::P:
                if (key_down_event.input_state.key_state.control())
                {
                    args.app.state = args.app.create_simulation_select_state();
                    return true;
                }
                return false;

            case lmpl::key_code::R:
                if (key_down_event.input_state.key_state.control())
                {

                    args.app.state = args.app.create_player_state();
                    return true;
                }
                return false;

            case lmpl::key_code::L:
                if (key_down_event.input_state.key_state.control())
                {
                    args.app.state.emplace<modal_state>(modal_state{
                      std::make_unique<map_selector>(
                        args.app.create_map_selector()),
                      [](editor_app &app, auto widget, auto &input_event) {
                          return dynamic_cast<map_selector *>(widget)->handle(
                            input_event,
                            map_selector_event_handler{
                              [&](map_selector_chose_map const &ev) {
                                  app.load_map(ev.path_to_file);
                                  app.state.emplace<gui_state>(app);
                              }});
                      }});
                    return true;
                }
                if (key_down_event.input_state.key_state.alt())
                {
                    args.app.toggle_tool_panel(args.app.entity_list.get());
                    return true;
                }
                return false;

            case lmpl::key_code::S:
                if (key_down_event.input_state.key_state.control())
                {
                    auto path =
                      args.app.map_file_project_relative_path.string();
                    args.app.state.emplace<modal_state>(modal_state{
                      .modal = std::make_unique<map_saver>(
                        args.app.resources,
                        path.substr(0, path.rfind(".lmap"))),
                      .input_handler = map_saver::handle,
                    });
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

    return args.app.input_handlers[args.app.visible_panels.front()](
      args.app, args.input_event);
}

gui_state::gui_state(editor_app &app) {}

void gui_state::add_to_frame(editor_app &app, lmgl::iframe *frame)
{
    for (auto &ppanel : app.visible_panels)
        ppanel->add_to_frame(frame, app);

    app.active_panel_border->add_to_frame(frame);
}

void gui_state::move_resources(editor_app &app) {}
} // namespace lmeditor
