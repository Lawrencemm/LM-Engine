#pragma once

#include "component.h"
#include <lmgl/lmgl.h>
#include <lmtk/font.h>
#include <lmtk/resource_cache.h>

namespace lmeditor
{
struct entity_list_init
{
    entt::registry &registry;
    lmgl::irenderer &renderer;
    lmtk::resource_cache const &resource_cache;
    lm::point2i position{0, 0};
    lm::size2i size{0, 0};

    component operator()();
};
} // namespace lmeditor
