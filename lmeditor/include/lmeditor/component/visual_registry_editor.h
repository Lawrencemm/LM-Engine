#pragma once

#include "component.h"
#include <bitset>
#include <entt/fwd.hpp>
#include <lmeditor/model/orbital_camera.h>
#include <lmtk/font.h>

namespace lmeditor
{
struct visual_registry_editor_capabilities
{
    enum class capabilities
    {
        add,
        copy,
        remove,
        translate,
        rotate,
        n_capabilities
    };

    void all() { flags.set(); }

    visual_registry_editor_capabilities &add()
    {
        flags.set(static_cast<size_t>(capabilities::add));
    }

    std::bitset<static_cast<size_t>(capabilities::n_capabilities)> flags;
};

struct visual_registry_editor_init
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
