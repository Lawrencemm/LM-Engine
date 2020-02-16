#pragma once

#include "mesh_editor.h"

#include <lmgl/mock_lmgl.h>

namespace lmeditor
{
const lm::size2i widget_size{1920, 1080};

inline bool operator==(
  const lmeditor::trimesh::Color &mesh_colour,
  const std::array<float, 3> arr)
{
    return mesh_colour == lmeditor::trimesh::Color{arr.data()};
}

class MeshEditorTestCase
{
  protected:
    lmeditor::trimesh mesh{lmeditor::trimesh::xy_plane(0.f, 1.f)};

    std::array<float, 3> fill_colour{1.f, 0.f, 0.f},
      unselected_colour{0.f, 1.f, 0.f}, selected_colour{0.f, 0.f, 1.f};

    lmgl::renderer renderer;
    class mesh_editor mesh_editor;

  public:
    MeshEditorTestCase()
        : renderer{std::make_unique<lmgl::MockRenderer>()},
          mesh_editor{mesh_editor_init{
            renderer.get(),
            widget_size,
            mesh,
            1.f,
            1.f,
            fill_colour,
            unselected_colour,
            selected_colour,
          }}
    {
    }
};
} // namespace lmeditor
