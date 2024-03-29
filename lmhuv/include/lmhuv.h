#pragma once

#include <optional>

#include <entt/entity/registry.hpp>

#include <lmgl/resource_sink.h>
#include <lmlib/camera.h>
#include <lmlib/geometry.h>
#include <lmlib/reference.h>

namespace lmhuv
{
class ivisual_view;

class ivisual_view
{
  public:
    virtual void update(
      entt::registry &registry,
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink) = 0;

    virtual void add_to_frame(
      entt::registry const &registry,
      lmgl::iframe *frame,
      lmgl::viewport const &viewport) = 0;

    virtual void move_resources(lmgl::resource_sink &resource_sink) = 0;

    virtual void
      recreate(entt::registry const &registry, lmgl::irenderer &renderer) = 0;

    virtual ivisual_view &set_camera_override(lm::camera const &camera) = 0;
};

using pvisual_view = lm::reference<ivisual_view>;

struct visual_view_init
{
    entt::registry &registry;
    lmgl::irenderer *renderer;
    float aspect_ratio{1.f};
    bool render_box_colliders{false};

    pvisual_view unique();
};

pvisual_view create_visual_view(visual_view_init const &init);
} // namespace lmhuv
