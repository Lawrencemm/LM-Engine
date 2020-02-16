#include <lmlib/range.h>
#include <trimesh.h>

namespace lmeditor
{

trimesh &trimesh::add_xy_plane(float z, float scale)
{
    auto extent = scale / 2.f;
    for (auto x : {extent, -extent})
        for (auto y : {extent, -extent})
            add_vertex({x, y, z});

    add_face_(3, 1, 0);
    add_face_(0, 2, 3);
    return *this;
}

trimesh::FaceHandle trimesh::add_face_(int v0, int v1, int v2)
{
    return add_face(VertexHandle{v0}, VertexHandle{v1}, VertexHandle{v2});
}

trimesh &trimesh::add_cube(float extent)
{
    for (auto x : {extent, -extent})
        for (auto y : {extent, -extent})
            for (auto z : {extent, -extent})
                add_vertex({x, y, z});

    // Right side.
    add_face_(0, 1, 3);
    add_face_(3, 2, 0);

    // Front.
    add_face_(1, 5, 7);
    add_face_(7, 3, 1);

    // Left.
    add_face_(7, 5, 4);
    add_face_(4, 6, 7);

    // Back.
    add_face_(6, 4, 0);
    add_face_(0, 2, 6);

    // Top.
    add_face_(0, 4, 5);
    add_face_(5, 1, 0);

    // Bottom.
    add_face_(2, 3, 7);
    add_face_(7, 6, 2);

    return *this;
}

trimesh &trimesh::add_box(float xextent, float yextent, float zextent)
{
    for (auto x : {xextent, -xextent})
        for (auto y : {yextent, -yextent})
            for (auto z : {zextent, -zextent})
                add_vertex({x, y, z});

    // Right side.
    add_face_(0, 1, 3);
    add_face_(3, 2, 0);

    // Front.
    add_face_(1, 5, 7);
    add_face_(7, 3, 1);

    // Left.
    add_face_(7, 5, 4);
    add_face_(4, 6, 7);

    // Back.
    add_face_(6, 4, 0);
    add_face_(0, 2, 6);

    // Top.
    add_face_(0, 4, 5);
    add_face_(5, 1, 0);

    // Bottom.
    add_face_(2, 3, 7);
    add_face_(7, 6, 2);

    return *this;
}

trimesh trimesh::xy_plane(float z, float scale)
{
    trimesh mesh;
    mesh.add_xy_plane(0.f, 5.f);
    return mesh;
}

trimesh trimesh::cube(float scale)
{
    trimesh mesh;
    mesh.add_cube(scale);
    return mesh;
}

trimesh trimesh::box(Eigen::Vector3f const &extents)
{
    trimesh mesh;
    mesh.add_box(extents[0], extents[1], extents[2]);
    return mesh;
}

OpenMesh::EdgeHandle
  trimesh::extrude_edge(const OpenMesh::EdgeHandle edge_handle)
{
    auto used_halfedge = halfedge_handle(edge_handle, 0);
    auto from_vertex = from_vertex_handle(used_halfedge);
    auto to_vertex = to_vertex_handle(used_halfedge);
    auto point_1 = point(from_vertex);
    auto new_vertex_1 = new_vertex(point_1);
    auto point_2 = point(to_vertex);
    auto new_vertex_2 = new_vertex(point_2);

    add_face(from_vertex, new_vertex_1, new_vertex_2);
    add_face(new_vertex_2, to_vertex, from_vertex);
    return this->edge_handle(halfedge_handle(new_vertex_2));
}

void trimesh::position_vertex(VertexHandle vh, const Point &new_pos) {}

std::pair<OpenMesh::VertexHandle, OpenMesh::VertexHandle>
  trimesh::get_vertices(OpenMesh::EdgeHandle eh) const
{
    auto heh = halfedge_handle(eh, 0);
    return {from_vertex_handle(heh), to_vertex_handle(heh)};
}

lm::array_proxy<char const> trimesh::points_data() const
{
    return lm::array_proxy<char const>(
      (char const *)points(), n_vertices() * sizeof(Point));
}

lm::array_proxy<char const> trimesh::colors_data()
{
    return lm::array_proxy<const char>(
      (char const *)vertex_colors(), n_vertices() * sizeof(Color));
}

trimesh_point trimesh::edge_midpoint(OpenMesh::EdgeHandle eh)
{
    auto [v1, v2] = get_vertices(eh);

    auto p1 = point(v1);
    auto p2 = point(v2);

    return p1 + (p2 - p1) / 2;
}

std::vector<lmeditor::trimesh::VertexHandle>
  gen_index_buffer(const lmeditor::trimesh &geo_mesh)
{
    std::vector<lmeditor::trimesh::VertexHandle> buffer;

    buffer.reserve(geo_mesh.n_faces() * 3);
    for (auto face : geo_mesh.faces())
    {
        for (auto face_vertex = geo_mesh.cfv_begin(face);
             face_vertex != geo_mesh.cfv_end(face);
             ++face_vertex)
        {
            buffer.emplace_back((*face_vertex).idx());
        }
    }
    return std::move(buffer);
}
} // namespace lmeditor
