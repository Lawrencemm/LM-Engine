#pragma once
#include <variant>
#include <vector>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <OpenMesh/Core/Geometry/EigenVectorT.hh>

#include "lmlib/geometry.h"
#include "lmlib/rotation_matrix.h"
#include <lmlib/array_proxy.h>

namespace lmhuv
{
class trimesh_point : public Eigen::Vector3f
{
    using Eigen::Vector3f::Vector3f;

  public:
    using vector_type = Eigen::Vector3f;
    using value_type = float;
    static const size_t size_ = 3;
};

struct trimesh_traits : public OpenMesh::DefaultTraits
{
    typedef lmhuv::trimesh_point Point;
    typedef lmhuv::trimesh_point Normal;
    typedef lmhuv::trimesh_point Color;

    VertexAttributes(OpenMesh::Attributes::Color);
    EdgeAttributes(OpenMesh::Attributes::Color);
};

class trimesh : public OpenMesh::TriMesh_ArrayKernelT<trimesh_traits>
{
  public:
    lm::array_proxy<char const> points_data() const;
    lm::array_proxy<char const> colors_data();
    trimesh &add_xy_plane(float z, float scale);
    trimesh &add_cube(float extent);
    trimesh &add_box(float xextent, float yextent, float zextent);
    static trimesh xy_plane(float z, float scale);
    static trimesh cube(float scale);
    static trimesh box(Eigen::Vector3f const &extents);

    std::pair<VertexHandle, VertexHandle> get_vertices(EdgeHandle) const;
    Point edge_midpoint(EdgeHandle eh);

    void position_vertex(VertexHandle vh, const Point &new_pos);
    EdgeHandle extrude_edge(const EdgeHandle edge_handle);

    struct geo_null_element
    {
    };
    struct vertex_geo_element
    {
        lmhuv::trimesh::VertexHandle vertex_handle;
    };
    struct edge_geo_element
    {
        lmhuv::trimesh::EdgeHandle edge_handle;
    };

    using element_variant =
      std::variant<geo_null_element, vertex_geo_element, edge_geo_element>;

  private:
    FaceHandle add_face_(int v0, int v1, int v2);
};
} // namespace lmhuv
