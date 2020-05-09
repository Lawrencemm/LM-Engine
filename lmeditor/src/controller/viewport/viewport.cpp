#include "viewport.h"

namespace lmeditor
{

void viewport::move_camera_closer(const float amount)
{
    camera.move(camera.get_distance() * -amount);
}

void viewport::move_camera_further(const float amount)
{
    camera.move(camera.get_distance() * amount);
}

viewport::viewport(viewport_init const &init) : camera{init.camera_init} {}

} // namespace lmeditor
