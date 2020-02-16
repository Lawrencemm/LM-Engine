#include "mesh_editor.h"
#include <numeric>

namespace lmeditor
{

OpenMesh::ArrayKernel::EdgeHandle mesh_editor::edge_hit_test(
  const lm::point2i &point,
  unsigned int radius_pixels) const
{
    auto closest =
      std::tuple{lmeditor::trimesh::EdgeHandle{-1}, (float)radius_pixels};

    for (const auto &eh : editable_mesh.geo_mesh.edges())
    {
        auto vh1 = editable_mesh.geo_mesh.from_vertex_handle(
          editable_mesh.geo_mesh.halfedge_handle(eh, 0));
        auto vh2 = editable_mesh.geo_mesh.to_vertex_handle(
          editable_mesh.geo_mesh.halfedge_handle(eh, 0));
        auto v1_ndc = camera.get_ndc(editable_mesh.geo_mesh.point(vh1));
        auto v2_ndc = camera.get_ndc(editable_mesh.geo_mesh.point(vh2));

        auto line = Eigen::ParametrizedLine<float, 2>::Through(
          v1_ndc.head(2), v2_ndc.head(2));

        auto real_point = get_surface_point_real(point);

        auto projection = line.projection(real_point);

        auto c_min_a = projection - v1_ndc.head(2);
        auto b_min_a = v2_ndc.head(2) - v1_ndc.head(2);
        auto dot = c_min_a.dot(b_min_a);

        if (dot < 0.f)
            continue;

        auto sq_len = b_min_a.squaredNorm();

        if (dot > sq_len)
            continue;

        auto offset = projection - real_point;

        auto pix_offset = Eigen::Vector2f(
          offset[0] / 2.f * size.width, offset[1] / 2.f * size.height);

        if (pix_offset.norm() <= std::get<1>(closest))
        {
            std::get<0>(closest) = eh;
            std::get<1>(closest) = pix_offset.norm();
        }
    }

    return std::get<0>(closest);
}

OpenMesh::ArrayKernel::VertexHandle mesh_editor::vertex_hit_test(
  const Eigen::Vector2f &surface_point_real,
  unsigned int radius_pixels) const
{
    auto closest_vertex = std::make_tuple(
      lmeditor::trimesh::VertexHandle{-1}, (float)radius_pixels);
    for (auto &vh : editable_mesh.geo_mesh.vertices())
    {
        auto vpos = editable_mesh.geo_mesh.point(vh);
        auto ndc_coord = camera.get_ndc(vpos);
        auto vpos_minus_ndc = ndc_coord.head(2) - surface_point_real;
        auto vpos_pix_offset = Eigen::Vector2f(
          vpos_minus_ndc[0] / 2.f * size.width,
          vpos_minus_ndc[1] / 2.f * size.height);
        if (vpos_pix_offset.norm() <= std::get<1>(closest_vertex))
        {
            std::get<0>(closest_vertex) = vh;
            std::get<1>(closest_vertex) = vpos_pix_offset.norm();
        }
    }
    return std::get<0>(closest_vertex);
}

lm::point2i mesh_editor::get_pix_coord(const lmeditor::trimesh_point &point)
{
    auto ndc = camera.get_ndc(point);

    return {int(size.width * ((ndc[0] + 1) / 2)),
            int(size.height * ((ndc[1] + 1) / 2))};
}

Eigen::Vector2f mesh_editor::get_surface_point_real(
  const Eigen::Vector2i &surface_point_pix) const
{
    auto surface_point_real =
      Eigen::Vector2f{surface_point_pix[0] / ((float)size.width / 2.f) - 1.f,
                      surface_point_pix[1] / ((float)size.height / 2.f) - 1.f};
    return surface_point_real;
}
} // namespace lmeditor