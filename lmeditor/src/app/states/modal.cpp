#include "../app.h"

namespace lmeditor
{
bool modal_state::handle_input_event(state_handle_args const &args)
{
    auto &app = args.app;
    if (
      args.input_event >> lm::variant_visitor{
                            [&](lmtk::key_down_event const &key_down_event) {
                                switch (key_down_event.key)
                                {
                                case lmpl::key_code::Escape:
                                    modal->move_resources(
                                      app.resources.renderer.get(),
                                      app.resources.resource_sink);
                                    app.state.emplace<gui_state>(app);
                                    return true;

                                default:
                                    return false;
                                }
                            },
                            [](auto) { return false; },
                          })
        return true;

    return input_handler(args.app, modal.get(), args.input_event);
}

void modal_state::move_resources(editor_app &app) {}

void modal_state::add_to_frame(editor_app &app, lmgl::iframe *frame)
{
    modal->add_to_frame(frame);
}
} // namespace lmeditor
