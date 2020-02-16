#pragma once

#include <iostream>
#include <optional>
#include <set>

#include <lmeditor/trimesh.h>
#include <lmlib/internal_fsm.h>

#include <lmeditor/mesh_editor.h>
#include <lmeditor/orbital_camera.h>

#include "../axes3d/axes3d.h"
#include "../editable_mesh/editable_mesh.h"
#include "../viewport/viewport.h"

namespace lmeditor
{

using vertex_set = std::set<editable_mesh::vertex_handle>;
using edge_set = std::set<editable_mesh::edge_handle>;

struct VertexModeBase
{
    vertex_set selection;

    template <typename handle_type> bool in_selection(handle_type vh)
    {
        return false;
    }
};

struct VertexSelectMode : public VertexModeBase
{
};
struct VertexMoveMode : public VertexModeBase
{
};

struct EdgeModeBase
{
    edge_set selection;

    template <typename handle_type> bool in_selection(handle_type vh)
    {
        return false;
    }
};
struct EdgeSelectMode : public EdgeModeBase
{
};
struct EdgeMoveMode : public EdgeModeBase
{
};

class mesh_editor : public lm::internal_fsm<
                      mesh_editor,
                      bool,
                      VertexSelectMode,
                      VertexMoveMode,
                      EdgeSelectMode,
                      EdgeMoveMode>,
                    public viewport
{
  public:
    explicit mesh_editor(const mesh_editor_init &init);

    lm::point2i get_pix_coord(const lmeditor::trimesh_point &point);
    mesh_editor &render(lmgl::iframe *frame);
    bool handle(const lmtk::input_event &event);
    mesh_editor &retire(lmgl::iframe *render_context);

    ~mesh_editor() = default;

    // Widget event handlers
    std::tuple<state_type_optional, bool>
      handle(VertexSelectMode &state, lmpl::mouse_button_down_message msg);

    std::tuple<state_type_optional, bool>
      handle(VertexSelectMode &state, lmpl::key_down_message msg);

    std::tuple<state_type_optional, bool>
      handle(EdgeSelectMode &state, lmpl::key_down_message msg);
    std::tuple<state_type_optional, bool>
      handle(EdgeSelectMode &state, lmpl::mouse_button_down_message msg);

    std::tuple<mesh_editor::state_type_optional, bool>
      handle(VertexMoveMode &state, lmpl::key_down_message msg);
    std::tuple<mesh_editor::state_type_optional, bool>
      handle(VertexMoveMode &state, const lmtk::mouse_move_event &message);

    std::tuple<mesh_editor::state_type_optional, bool>
      handle(EdgeMoveMode &state, const lmtk::mouse_move_event &message);
    std::tuple<mesh_editor::state_type_optional, bool>
      handle(EdgeMoveMode &state, lmpl::key_down_message message);

    // Default handler for widget events, do nothing.
    template <typename state_type, typename event_type>
    std::tuple<mesh_editor::state_type_optional, bool>
      handle(state_type, event_type)
    {
        return {std::nullopt, false};
    }

    // Move geometry
    mesh_editor &
      translate_vertex(OpenMesh::VertexHandle handle, const Eigen::Vector3f &f);

    /// Move vertex in the camera's orthonormal basis to a pixel coordinate.
    void move_vertex_to(
      lmeditor::trimesh::VertexHandle vh,
      Eigen::Vector2i pix_coord);

    /// Move vertex in the camera's orthonormal basis by a pixel offset.
    void move_vertex(
      lmeditor::trimesh::VertexHandle vh,
      Eigen::Vector2i pix_offset);

    void move_edge(OpenMesh::EdgeHandle eh, Eigen::Vector2i pix_offset);

    // UI
    lm::size2i size;

    lmgl::irenderer *renderer;

    lmpl::key_state key_state;

    // Hit testing

    OpenMesh::VertexHandle vertex_hit_test(
      const Eigen::Vector2f &surface_point_real,
      unsigned int radius_pixels) const;

    lmeditor::trimesh::EdgeHandle
      edge_hit_test(const lm::point2i &point, unsigned int radius_pixels) const;

    /// Convert a pixel surface point to a normalised float surface point.
    Eigen::Vector2f
      get_surface_point_real(const Eigen::Vector2i &surface_point_pix) const;

    lm::point2i point(editable_mesh::vertex_handle vh);
    lm::point2i point(editable_mesh::edge_handle eh);
    axes axes_;
    class editable_mesh editable_mesh;

    std::array<float, 3> selected_colour;
    std::array<float, 3> unselected_colour;

    template <typename state_type> void on_clicked_nothing(state_type &state);
    template <typename state_type, typename handle_type>
    void on_clicked(state_type &state, handle_type handle);

    template <typename state_type, typename handle_type>
    void modify_selection(state_type &state, handle_type h);
    template <typename state_type, typename handle_type>
    void select_only(state_type &state, handle_type h);
    template <typename state_type, typename handle_type>
    void select_only(handle_type h);
    template <typename state_type> void clear_selection(state_type &state);

    template <typename handle_type> bool in_selection(handle_type handle);
}; // class mesh_editor
} // namespace lmeditor
