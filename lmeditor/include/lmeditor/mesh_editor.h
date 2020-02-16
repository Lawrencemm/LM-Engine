#pragma once

#include <lmeditor/trimesh.h>
#include <lmgl/lmgl.h>
#include <lmpl/lmpl.h>
#include <lmtk/lmtk.h>

namespace lmeditor
{
struct mesh_editor_init
{
    lmgl::irenderer *renderer;
    const lm::size2i size;
    lmeditor::trimesh &mesh;
    float point_size, line_width;
    std::array<float, 3> fill_colour, unselected_colour, selected_colour;
};
} // namespace lmeditor
