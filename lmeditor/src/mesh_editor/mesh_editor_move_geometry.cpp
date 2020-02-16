#include "mesh_editor.h"

namespace lmeditor
{

void mesh_editor::move_edge(OpenMesh::EdgeHandle eh, Eigen::Vector2i pix_offset)
{
    auto vertex_handles = editable_mesh.geo_mesh.get_vertices(eh);

    move_vertex(vertex_handles.first, pix_offset);
    move_vertex(vertex_handles.second, pix_offset);
}

void mesh_editor::move_vertex_to(
  lmeditor::trimesh::VertexHandle vh,
  Eigen::Vector2i pix_coord)
{
    lmeditor::trimesh_point &vpos = editable_mesh.geo_mesh.point(vh);
    auto vpos_ndc = camera.get_ndc(vpos);

    auto new_surface_real = get_surface_point_real(pix_coord);
    Eigen::Vector4f new_ndc;
    // The end point has the same screen depth since the mouse doesn't move in
    // screen z.
    new_ndc << new_surface_real, vpos_ndc[2], vpos_ndc[3];

    // Get the new point in clip space.
    Eigen::Vector4f new_clip;
    new_clip << new_ndc.head(3) * new_ndc[3], new_ndc[3];

    // Get the two points in world space.
    Eigen::Vector4f new_world = camera.view_matrix().inverse() * new_clip;

    // Set vertex position to the new point.
    vpos = new_world.head(3);
}

void mesh_editor::move_vertex(
  lmeditor::trimesh::VertexHandle vh,
  Eigen::Vector2i pix_offset)
{
    lmeditor::trimesh_point &vpos = editable_mesh.geo_mesh.point(vh);
    auto vpos_ndc = camera.get_ndc(vpos);

    auto new_ndc =
      vpos_ndc + Eigen::Vector4f{((float)pix_offset[0] / size.width) * 2,
                                 ((float)pix_offset[1] / size.height) * 2,
                                 0.f,
                                 0.f};

    // Get the new point in clip space.
    Eigen::Vector4f new_clip;
    new_clip << new_ndc.head(3) * new_ndc[3], new_ndc[3];

    // Get the two points in world space.
    Eigen::Vector4f new_world = camera.view_matrix().inverse() * new_clip;

    // Set vertex position to the new point.
    vpos = new_world.head(3);
}

mesh_editor &mesh_editor::translate_vertex(
  OpenMesh::VertexHandle handle,
  const Eigen::Vector3f &f)
{

    editable_mesh.geo_mesh.point(handle) += f;
    return *this;
}
} // namespace lmeditor
