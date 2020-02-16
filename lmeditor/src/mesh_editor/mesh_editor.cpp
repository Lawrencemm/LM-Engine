#include <iostream>
#include <numeric>

#include <Resource.h>

#include <lmgl/renderer.h>

#include "../viewport/viewport.h"
#include "mesh_editor.h"

namespace lmeditor
{
mesh_editor::mesh_editor(const mesh_editor_init &init)
    : viewport{{{
        .fov = (float)M_PI / 3,
        .aspect = (float)init.size.width / init.size.height,
        .near_clip = 0.1f,
        .far_clip = 1000.f,
        .position = Eigen::Vector3f{0.f, 5.f, -15.f},
        .target = Eigen::Vector3f{0.f, 0.f, 0.f},
      }}},
      renderer{init.renderer},
      size{init.size},
      axes_{{
        .renderer = init.renderer,
        .extent = 100.f,
        .x_axis_colour = {0.65f, 0.f, 0.f},
        .z_axis_colour = {0.f, 0.f, 0.55f},
        .line_width = init.line_width,
      }},
      editable_mesh{{
        .renderer = init.renderer,
        .geo_mesh = init.mesh,
        .solid_colour = init.fill_colour,
        .line_colour = init.unselected_colour,
        .point_colour = init.unselected_colour,
        .line_width = init.line_width,
        .point_size = init.point_size,
      }},
      unselected_colour{init.unselected_colour},
      selected_colour{init.selected_colour}
{
}

mesh_editor &mesh_editor::retire(lmgl::iframe *render_context)
{
    axes_.on_render(render_context, camera);
    editable_mesh.render(render_context, camera);
    return *this;
}

mesh_editor &mesh_editor::render(lmgl::iframe *frame)
{
    editable_mesh.on_complete(frame);
    return *this;
}

bool mesh_editor::handle(const lmtk::input_event &event)
{
    // Dispatch messages through internal_fsm.
    bool dirty =
      event >> lm::variant_visitor{[this](auto &msg) { return dispatch(msg); }};
    dirty |= event >> lm::variant_visitor{[this](auto msg) {
                 return viewport::control_camera(msg);
             }};

    return dirty;
}

lm::point2i mesh_editor::point(editable_mesh::vertex_handle vh)
{
    return get_pix_coord(editable_mesh.geo_mesh.point(vh));
}

lm::point2i mesh_editor::point(editable_mesh::edge_handle eh)
{
    return get_pix_coord(editable_mesh.geo_mesh.edge_midpoint(eh));
}

template <>
bool EdgeModeBase::in_selection<editable_mesh::edge_handle>(
  editable_mesh::edge_handle vh)
{
    return selection.find(vh) != selection.end();
}

template <>
bool VertexModeBase::in_selection<editable_mesh::vertex_handle>(
  editable_mesh::vertex_handle vh)
{
    return selection.find(vh) != selection.end();
}
} // namespace lmeditor
