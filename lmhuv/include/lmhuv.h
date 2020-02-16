#pragma once

#include <optional>

#include <entt/entity/registry.hpp>

#include <lmlib/camera.h>
#include <lmlib/geometry.h>
#include <lmlib/reference.h>
#include <lmtk/lmtk.h>
#include <lmtk/resource_sink.h>

namespace lmhuv
{
class ivisual_view;

struct visual_view_connections
{
  private:
    ivisual_view &visual_view;
    lmgl::irenderer &renderer;
    lmtk::resource_sink &resource_sink;

    entt::scoped_connection box_construct, box_remove, box_collider_construct,
      box_collider_remove;

  public:
    visual_view_connections(
      ivisual_view &visual_view,
      entt::registry &registry,
      lmgl::irenderer &renderer,
      lmtk::resource_sink &resource_sink);

    void add_box(entt::entity entity, entt::registry &registry);
    void remove_box(entt::entity entity, entt::registry &registry);

    void add_box_collider(entt::entity entity, entt::registry &registry);
    void remove_box_collider(entt::entity entity, entt::registry &registry);
};

class ivisual_view
{
  public:
    virtual void add_to_frame(
      entt::registry const &registry,
      lmgl::iframe *frame,
      lmgl::viewport const &viewport) = 0;

    virtual void move_resources(
      lmtk::resource_sink &resource_sink,
      lmgl::irenderer *renderer) = 0;

    virtual ivisual_view &
      clear(lmgl::irenderer *renderer, lmtk::resource_sink &resource_sink) = 0;

    virtual void add_box(lmgl::irenderer *renderer, entt::entity entity) = 0;
    virtual void
      add_box_wireframe(lmgl::irenderer *renderer, entt::entity entity) = 0;

    virtual void destroy_box(
      lmgl::irenderer *renderer,
      entt::entity entity,
      lmtk::resource_sink &resource_sink) = 0;

    virtual void destroy_box_wireframe(
      lmgl::irenderer *renderer,
      entt::entity entity,
      lmtk::resource_sink &resource_sink) = 0;

    virtual void
      recreate(entt::registry const &registry, lmgl::irenderer &renderer) = 0;

    virtual ivisual_view &set_camera_override(lm::camera const &camera) = 0;
};

using pvisual_view = lm::reference<ivisual_view>;

struct visual_view_init
{
    entt::registry const &registry;
    lmgl::irenderer *renderer;
    float aspect_ratio;
    bool render_box_colliders{false};
};

pvisual_view create_visual_view(visual_view_init const &init);
} // namespace lmhuv
