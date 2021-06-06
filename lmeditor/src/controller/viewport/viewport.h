#pragma once

#include <lmeditor/model/orbital_camera.h>

namespace lmeditor
{
struct viewport_init;
class viewport
{
  public:
    viewport(viewport_init const &init);

    void move_camera_closer(float amount);
    void move_camera_further(float amount);

    orbital_camera camera;
};

struct viewport_init
{
    orbital_camera_init const camera_init;
};
} // namespace lmeditor
