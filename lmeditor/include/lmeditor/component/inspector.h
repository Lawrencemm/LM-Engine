#pragma once

#include "component.h"
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <lmlib/reference.h>
#include <lmtk/input_event.h>
#include <lmtk/text_layout.h>
#include <variant>

namespace lmeditor
{
struct inspector_interface : public component_interface
{
    virtual void
      display(entt::registry const &registry, entt::entity entity) = 0;

    virtual void update(entt::registry &registry) = 0;

    virtual void clear() = 0;
};

using inspector = lm::reference<inspector_interface>;

struct inspector_init
{
    entt::registry &registry;
    lmgl::irenderer &renderer;
    lmgl::material text_material;
    lmtk::ifont const *font;
    lm::size2i const &size;

    inspector operator()();
};
} // namespace lmeditor
