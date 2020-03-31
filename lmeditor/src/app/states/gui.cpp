#include "../app.h"
#include "../saver.h"
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
                args.app.init_command_help();
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
                    args.app.init_map_selector();
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
                    args.app.init_map_saver();
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
