#pragma once

#include <Eigen/Eigen>
#include <entt/entt.hpp>
#include <lmgl/fwd_decl.h>
#include <lmgl/resource_sink.h>
#include <lmhuv.h>
#include <lmlib/camera.h>

namespace lmhuv::internal
{
class visual : public ivisual_view
{
  public:
    // We're attaching to registry with signals, don't
    // allow moving which would invalidate those.
    visual(visual &&) = delete;
    visual(visual const &) = delete;

    void move_resources(lmgl::resource_sink &resource_sink) override;

  public:
    void add_to_frame(
      entt::registry const &registry,
      lmgl::iframe *frame,
      lmgl::viewport const &viewport) override;

    visual(visual_view_init const &init);
    ivisual_view &set_camera_override(lm::camera const &camera) override;
    void recreate(entt::registry const &registry, lmgl::irenderer &renderer)
      override;
    void update(
      entt::registry &registry,
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink) override;

  private:
    lmgl::material box_material, box_wireframe_material;
    lmgl::buffer box_vpositions, box_vnormals, box_indices,
      box_instances_vbuffer, box_collider_instances_vbuffer, boxes_ubuffer;
    lmgl::geometry boxes_geometry, box_colliders_geometry;
    size_t n_box_indices;

    Eigen::Vector3f light_direction;
    float aspect_ratio;
    std::optional<lm::camera> camera_override;
    bool do_render_box_colliders;
};
} // namespace lmhuv::internal
