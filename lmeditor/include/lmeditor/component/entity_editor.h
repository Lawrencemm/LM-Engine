#pragma once

#include "component.h"
#include <bitset>
#include <entt/fwd.hpp>
#include <lmeditor/model/orbital_camera.h>
#include <lmtk/font.h>

namespace lmeditor
{
struct entity_editor_features
{
    enum class flag
    {
        // Add non-root entities ie. new children
        add,
        // Add root entities ie. without a parent
        add_root,
        reparent,
        rotate,
        scale,
        translate,
        n_features
    };

    entity_editor_features &add()
    {
        flags.set(static_cast<size_t>(flag::add));
        return *this;
    }

    entity_editor_features &add_root()
    {
        flags.set(static_cast<size_t>(flag::add_root));
        return *this;
    }

    entity_editor_features &reparent()
    {
        flags.set(static_cast<size_t>(flag::reparent));
        return *this;
    }

    entity_editor_features &rotate()
    {
        flags.set(static_cast<size_t>(flag::rotate));
        return *this;
    }

    entity_editor_features &scale()
    {
        flags.set(static_cast<size_t>(flag::scale));
        return *this;
    }

    entity_editor_features &translate()
    {
        flags.set(static_cast<size_t>(flag::translate));
        return *this;
    }

    std::bitset<static_cast<size_t>(flag::n_features)> flags;
};

struct entity_editor_init
{
    entt::registry &registry;
    orbital_camera_init camera_init;
    lmgl::irenderer *renderer;
    lmtk::resource_cache const &resource_cache;
    lm::point2i position;
    lm::size2i size;
    std::array<float, 3> selection_outline_colour;
    entity_editor_features features;

    component operator()();
};
} // namespace lmeditor
