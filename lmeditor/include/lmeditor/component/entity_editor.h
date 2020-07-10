#pragma once

#include "component.h"
#include <entt/fwd.hpp>
#include <lmeditor/model/orbital_camera.h>
#include <lmtk/font.h>

namespace lmeditor
{
struct entity_editor_init
{
    entt::registry &registry;
    orbital_camera_init camera_init;
    lmgl::irenderer *renderer;
    lmtk::resource_cache const &resource_cache;
    lm::point2i position;
    lm::size2i size;
    std::array<float, 3> selection_outline_colour;

    component operator()();
};
} // namespace lmeditor
