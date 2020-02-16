#pragma once

#include <lmeditor/orbital_camera.h>
#include <lmtk/lmtk.h>

namespace lmeditor
{
struct viewport_init;
class viewport
{
  public:
    viewport(viewport_init const &init);
    bool handle(const lmtk::input_event &event);

    void move_camera_closer(const float amount);
    void move_camera_further(const float amount);

    bool control_camera(const lmtk::key_down_event &msg);
    bool control_camera(const lmtk::mouse_move_event &msg);
    bool control_camera(const lmtk::mouse_button_down_event &msg);
    template <typename message_type> bool control_camera(message_type msg)
    {
        return false;
    }

    orbital_camera camera;
};

struct viewport_init
{
    orbital_camera_init const camera_init;
};
} // namespace lmeditor
