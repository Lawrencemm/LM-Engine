#include <Eigen/Eigen>
#include <entt/entt.hpp>
#include <lmgl/fwd_decl.h>
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

    void move_resources(
      lmtk::resource_sink &resource_sink,
      lmgl::irenderer *renderer) override;

    visual &clear(lmgl::irenderer *renderer, lmtk::resource_sink &resource_sink)
      override;

  public:
    void add_to_frame(
      entt::registry const &registry,
      lmgl::iframe *frame,
      lmgl::viewport const &viewport) override;

    visual(visual_view_init const &init);
    void add_box(lmgl::irenderer *renderer, entt::entity entity);
    void add_box_wireframe(lmgl::irenderer *renderer, entt::entity entity);
    void destroy_box(
      lmgl::irenderer *renderer,
      entt::entity entity,
      lmtk::resource_sink &resource_sink);
    ivisual_view &set_camera_override(lm::camera const &camera) override;
    void recreate(entt::registry const &registry, lmgl::irenderer &renderer)
      override;
    void destroy_box_collider_mesh(
      lmgl::irenderer *renderer,
      entt::entity entity,
      lmtk::resource_sink &resource_sink);
    void update(
      entt::registry &registry,
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;

  private:
    lmgl::material box_material, box_wireframe_material;
    lmgl::buffer box_vpositions, box_vnormals, box_indices;
    size_t n_box_indices;
    struct box_mesh
    {
        lmgl::buffer ubuffer;
        lmgl::geometry geometry;
    };
    std::unordered_map<entt::entity, box_mesh> box_meshes;
    std::unordered_map<entt::entity, box_mesh> box_collider_meshes;
    Eigen::Vector3f light_direction;
    float aspect_ratio;
    std::optional<lm::camera> camera_override;
    bool do_render_box_colliders;

    entt::observer box_render_observer, box_collider_observer;

    entt::scoped_connection box_render_destroyed_connection,
      box_collider_destroyed_connection, box_mesh_tag_destroyed_connection,
      box_collider_mesh_tag_destroyed_connection;

    std::vector<entt::entity> destroyed_box_meshes,
      destroyed_box_collider_meshes;

    void handle_box_mesh_tag_destroyed(
      entt::registry &registry,
      entt::entity entity);

    void handle_box_collider_mesh_tag_destroyed(
      entt::registry &registry,
      entt::entity entity);

    box_mesh create_box_mesh(
      lmgl::irenderer *renderer,
      entt::entity &entity,
      lmgl::material material);

    void
      add_box_meshes(entt::registry const &registry, lmgl::irenderer *renderer);
    void render_boxes(
      entt::registry const &registry,
      lm::camera const &camera,
      lmgl::iframe *frame,
      lmgl::viewport const &viewport);
    void add_box_collider_meshes(
      entt::registry const &registry,
      lmgl::irenderer &renderer);
    void render_box_colliders(
      entt::registry const &registry,
      lm::camera const &camera,
      lmgl::iframe *frame,
      lmgl::viewport const &viewport);
};
} // namespace lmhuv::internal
