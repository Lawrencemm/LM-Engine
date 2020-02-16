#include "../viewport/viewport.h"
#include <lmlib/variant_visitor.h>

namespace lmeditor
{

bool viewport::control_camera(const lmtk::mouse_move_event &msg)
{
    if (msg.input_state.mouse_state[lmpl::mouse_button::M])
    {
        auto delta_mouse_pos = msg.delta;

        if (msg.delta.width || msg.delta.height)
        {
            camera.rotate({delta_mouse_pos.height * 0.002f,
                           delta_mouse_pos.width * 0.002f});
            return true;
        }
    }
    return false;
}

bool viewport::control_camera(const lmtk::mouse_button_down_event &msg)
{
    switch (msg.button)
    {
    case lmpl::mouse_button::U:
        move_camera_closer(0.1f);
        return true;

    case lmpl::mouse_button::D:
        move_camera_further(0.1f);
        return true;

    default:
        return false;
    }
}

bool viewport::control_camera(const lmtk::key_down_event &msg)
{
    if (msg.input_state.key_state.have_modifiers())
        return false;

    switch (msg.key)
    {
    case lmpl::key_code::F:
        camera.rotate({0.f, -0.05f});
        return true;

    case lmpl::key_code::D:
        camera.rotate({-0.05f, 0.f});
        return true;

    case lmpl::key_code::S:
        camera.rotate({0.f, 0.05f});
        return true;

    case lmpl::key_code::E:
        camera.rotate({0.05f, 0.f});
        return true;

    case lmpl::key_code::R:
        move_camera_closer(0.1f);
        return true;

    case lmpl::key_code::W:
        move_camera_further(0.1f);
        return true;

    default:
        return false;
    }
}

void viewport::move_camera_closer(const float amount)
{
    camera.move(camera.get_distance() * -amount);
}

void viewport::move_camera_further(const float amount)
{
    camera.move(camera.get_distance() * amount);
}

viewport::viewport(viewport_init const &init) : camera{init.camera_init} {}

bool viewport::handle(const lmtk::input_event &event)
{
    bool dirty = event >> lm::variant_visitor{
                            [this](auto msg) { return control_camera(msg); },
                          };
    return dirty;
}
} // namespace lmeditor
