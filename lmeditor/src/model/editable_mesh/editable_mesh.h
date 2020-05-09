#pragma once

#include <queue>
#include <set>
#include <variant>

#include <lmgl/lmgl.h>
#include <lmlib/camera.h>

#include <lmeditor/model/trimesh.h>

namespace lmeditor
{

struct editable_mesh_init;

class editable_mesh
{
  public:
    explicit editable_mesh(const editable_mesh_init &init);
    void render(lmgl::iframe *frame, const lm::camera &camera);
    void on_complete(lmgl::iframe *frame);

  private:
    void init_geometry(const editable_mesh_init &init);
    void init_vcolours(std::array<float, 3> colour);
    void init_ecolours(std::array<float, 3> array);

    void init_vertex_buffers();
    void init_common_mesh_properties();

    void regenerate_mesh_rendering_data();

    void init_rendering(const editable_mesh_init &init);
    void init_uniform_buffers(const editable_mesh_init &init);
    void init_mesh_index_buffers();
    void init_tri_mesh();
    void init_points_mesh(const editable_mesh_init &init);
    void init_lines_meshes(const editable_mesh_init &init);

  public:
    using vertex_handle = lmeditor::trimesh::VertexHandle;
    using edge_handle = lmeditor::trimesh::EdgeHandle;

    editable_mesh &set_colour(vertex_handle vh, std::array<float, 3> colour);
    editable_mesh &set_colour(edge_handle eh, std::array<float, 3> colour);

    lmeditor::trimesh::EdgeHandle
      extrude_edge(lmeditor::trimesh::EdgeHandle edge_handle);

    lmgl::index_type get_mesh_index_type();

    // Geometry
    lmeditor::trimesh &geo_mesh;
    lmgl::irenderer *renderer;

    std::vector<lmeditor::trimesh::VertexHandle> mesh_indices;

    std::vector<lmeditor::trimesh::VertexHandle> edge_indices;
    std::vector<lmeditor::trimesh::Point> edge_mesh_positions;
    std::vector<lmeditor::trimesh::Color> edge_mesh_colours;

    // Graphics
    lmgl::buffer vpositions_buffer;
    lmgl::buffer vcolours_buffer;
    lmgl::buffer vpositions_edge_buffer;
    lmgl::buffer vcolours_edge_buffer;

    lmgl::buffer mesh_index_buffer;
    lmgl::buffer edge_index_buffer;

    lmgl::buffer tris_ubuffer;
    lmgl::buffer edge_ubuffer;
    lmgl::buffer points_ubuffer;

    lmgl::geometry mesh_render_geometry;
    lmgl::geometry points_render_geometry;
    lmgl::geometry lines_render_geometry;

    std::queue<lmgl::iframe *> in_flight_frames;
    std::unordered_map<lmgl::iframe *, std::array<lmgl::buffer, 6>>
      frame_resources;
};

struct editable_mesh_init
{
    lmgl::irenderer *renderer;
    lmeditor::trimesh &geo_mesh;
    std::array<float, 3> solid_colour, line_colour, point_colour;
    float line_width, point_size;
};

/// Generate an index buffer for a halfedge mesh to render triangles/points.
std::vector<lmeditor::trimesh::VertexHandle>
  gen_index_buffer(const lmeditor::trimesh &geo_mesh);

/// Generate an index buffer for a halfedge mesh to render edges.
std::tuple<
  std::vector<lmeditor::trimesh::Point>,
  std::vector<lmeditor::trimesh::Color>,
  std::vector<lmeditor::trimesh::VertexHandle>>
  gen_edge_mesh_data(const lmeditor::trimesh &geo_mesh);
} // namespace lmeditor
