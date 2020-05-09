#include "editable_mesh.h"

#include <lmlib/enumerate.h>

namespace lmeditor
{
void editable_mesh::init_geometry(const editable_mesh_init &init)
{
    init_vcolours(init.point_colour);
    init_ecolours(init.line_colour);
    regenerate_mesh_rendering_data();
}

std::tuple<
  std::vector<lmeditor::trimesh::Point>,
  std::vector<lmeditor::trimesh::Color>,
  std::vector<lmeditor::trimesh::VertexHandle>>
  gen_edge_mesh_data(const lmeditor::trimesh &geo_mesh)
{
    std::vector<lmeditor::trimesh::VertexHandle> indices;
    std::vector<lmeditor::trimesh::Point> points;
    std::vector<lmeditor::trimesh::Color> colours;

    indices.reserve(geo_mesh.n_edges() * 2);
    for (auto [i, edge] :
         lm::enumerate(geo_mesh.edges_begin(), geo_mesh.edges_end()))
    {
        auto [v1, v2] = geo_mesh.get_vertices(edge);

        points.emplace_back(geo_mesh.point(v1));
        points.emplace_back(geo_mesh.point(v2));

        colours.emplace_back(geo_mesh.color(edge));
        colours.emplace_back(geo_mesh.color(edge));

        indices.emplace_back(2 * i);
        indices.emplace_back(2 * i + 1);
    }
    return std::tuple{
      std::move(points), std::move(colours), std::move(indices)};
}

void editable_mesh::regenerate_mesh_rendering_data()
{
    mesh_indices = gen_index_buffer(geo_mesh);
    std::tie(edge_mesh_positions, edge_mesh_colours, edge_indices) =
      std::move(gen_edge_mesh_data(geo_mesh));
}

void editable_mesh::init_vcolours(std::array<float, 3> colour)
{
    for (auto &vh : geo_mesh.vertices())
    {
        geo_mesh.set_color(vh, Eigen::Vector3f{colour.data()});
    }
}

void editable_mesh::init_ecolours(std::array<float, 3> array)
{
    for (auto eh : geo_mesh.edges())
    {
        geo_mesh.set_color(eh, lmeditor::trimesh::Color{array.data()});
    }
}

lmeditor::trimesh::EdgeHandle
  editable_mesh::extrude_edge(lmeditor::trimesh::EdgeHandle edge_handle)
{
    return geo_mesh.extrude_edge(edge_handle);
}

lmgl::geometry_index_type editable_mesh::get_mesh_index_type()
{
    constexpr auto size = sizeof(lmeditor::trimesh::VertexHandle);
    static_assert(
      (size == 4) || (size == 2), "Mesh vertex handle is not 16 or 32 bits.");
    if constexpr (size == 4)
        return lmgl::geometry_index_type::four_bytes;
    if constexpr (size == 2)
        return lmgl::geometry_index_type::two_bytes;
}
} // namespace lmeditor
