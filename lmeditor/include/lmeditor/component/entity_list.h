#pragma once

#include "component.h"
#include <lmgl/lmgl.h>
#include <lmtk/font.h>

namespace lmeditor
{
struct entity_list_init
{
    entt::registry &registry;
    lmgl::irenderer &renderer;
    lmgl::material text_material, rect_material;
    lmtk::ifont const *font;
    lm::point2i position{0, 0};
    lm::size2i size{0, 0};

    component operator()();
};
} // namespace lmeditor
