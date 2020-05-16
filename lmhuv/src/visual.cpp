#include "visual.h"
#include "rendering/shapes.h"
#include <lmengine/camera.h>
#include <lmengine/physics.h>
#include <lmhuv.h>
#include <lmhuv/box.h>

namespace lmhuv::internal
{
struct box_mesh_tag
{
};
struct box_collider_mesh_tag
{
};

visual::visual(visual_view_init const &init)
    : box_material{box_material_init{*init.renderer}()},
      box_wireframe_material{box_material_init{
        .renderer = *init.renderer,
        .polygon_mode = lmgl::polygon_mode::line,
      }()},
      light_direction{Eigen::Vector3f{0.2f, 0.6f, -0.75f}.normalized()},
      aspect_ratio{init.aspect_ratio},
      do_render_box_colliders{init.render_box_colliders},
      box_render_observer{
        init.registry,
        entt::collector.group<lmng::box_render>()},
      box_collider_observer{
        init.registry,
        entt::collector.group<lmng::box_collider>()},
      box_render_destroyed_connection{
        init.registry.on_destroy<lmng::box_render>()
          .connect<&entt::registry::remove_if_exists<box_mesh_tag>>()},
      box_collider_destroyed_connection{
        init.registry.on_destroy<lmng::box_collider>()
          .connect<&entt::registry::remove_if_exists<box_collider_mesh_tag>>()},
      box_mesh_tag_destroyed_connection{
        init.registry.on_destroy<box_mesh_tag>()
          .connect<&visual::handle_box_mesh_tag_destroyed>(this)},
      box_collider_mesh_tag_destroyed_connection{
        init.registry.on_destroy<box_collider_mesh_tag>()
          .connect<&visual::handle_box_collider_mesh_tag_destroyed>(this)}
{
    std::tie(box_vpositions, box_vnormals, box_indices, n_box_indices) =
      lmhuv::create_box_buffers(init.renderer);

    recreate(init.registry, *init.renderer);
}

void visual::update(
  entt::registry &registry,
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    for (auto entity : box_render_observer)
    {
        add_box(renderer, entity);
        registry.assign<box_mesh_tag>(entity);
    }
    box_render_observer.clear();

    for (auto entity : box_collider_observer)
    {
        add_box_wireframe(renderer, entity);
        registry.assign<box_collider_mesh_tag>(entity);
    }
    box_collider_observer.clear();

    for (auto entity : destroyed_box_meshes)
    {
        destroy_box(renderer, entity, resource_sink);
    }
    destroyed_box_meshes.clear();

    for (auto entity : destroyed_box_collider_meshes)
    {
        destroy_box_collider_mesh(renderer, entity, resource_sink);
    }
    destroyed_box_collider_meshes.clear();
}

void visual::add_box_meshes(
  entt::registry const &registry,
  lmgl::irenderer *renderer)
{
    registry.view<lmng::box_render const>().each(
      [&](entt::entity entity, lmng::box_render const &box_component) {
          box_meshes.emplace(
            entity, create_box_mesh(renderer, entity, box_material));
      });
}

visual::box_mesh visual::create_box_mesh(
  lmgl::irenderer *renderer,
  entt::entity &entity,
  lmgl::material material)
{
    auto ubuffer = create_box_ubuffer(renderer);
    auto geometry = renderer->create_geometry(lmgl::geometry_init{
      .material = material,
      .vertex_buffers =
        {
          box_vpositions.get(),
          box_vnormals.get(),
        },
      .index_buffer = box_indices.get(),
      .uniform_buffer = ubuffer.get(),
    });
    geometry->set_n_indices(n_box_indices);
    return box_mesh{
      std::move(ubuffer),
      std::move(geometry),
    };
}

void visual::add_to_frame(
  entt::registry const &registry,
  lmgl::iframe *frame,
  lmgl::viewport const &viewport)
{
    if (camera_override)
    {
        render_boxes(registry, camera_override.value(), frame, viewport);
        if (do_render_box_colliders)
            render_box_colliders(
              registry, camera_override.value(), frame, viewport);
        return;
    }

    registry.view<lmng::camera const, lmng::transform const>().each(
      [&](auto camera_entity, auto &camera_component, auto &transform) {
          auto resolved_transform =
            lmng::resolve_transform(registry, camera_entity);
          lm::camera camera{lm::camera_init{
            camera_component.field_of_view,
            aspect_ratio,
            camera_component.near_clip,
            camera_component.far_clip,
            resolved_transform.position,
            resolved_transform.rotation,
          }};
          render_boxes(registry, camera, frame, viewport);

          if (do_render_box_colliders)
              render_box_colliders(registry, camera, frame, viewport);
      });
}

void visual::move_resources(
  lmgl::resource_sink &resource_sink,
  lmgl::irenderer *renderer)
{
    resource_sink.add(
      renderer, box_vpositions, box_vnormals, box_indices, box_material);
    clear(renderer, resource_sink);
}

void visual::add_box(lmgl::irenderer *renderer, entt::entity entity)
{
    box_meshes.emplace(entity, create_box_mesh(renderer, entity, box_material));
}

void visual::add_box_wireframe(lmgl::irenderer *renderer, entt::entity entity)
{
    box_collider_meshes.emplace(
      entity, create_box_mesh(renderer, entity, box_wireframe_material));
}

void visual::destroy_box(
  lmgl::irenderer *renderer,
  entt::entity entity,
  lmgl::resource_sink &resource_sink)
{
    auto &mesh = box_meshes.at(entity);
    resource_sink.add(renderer, mesh.ubuffer);
    resource_sink.add(renderer, mesh.geometry);
    box_meshes.erase(entity);
}

visual &
  visual::clear(lmgl::irenderer *renderer, lmgl::resource_sink &resource_sink)
{
    for (auto &key_val : box_meshes)
        resource_sink.add(
          renderer, key_val.second.ubuffer, key_val.second.geometry);
    return *this;
}

void visual::render_box_colliders(
  entt::registry const &registry,
  lm::camera const &camera,
  lmgl::iframe *frame,
  lmgl::viewport const &viewport)
{
    registry.view<lmng::box_collider const, lmng::transform const>().each(
      [&](
        auto entity,
        lmng::box_collider const &box,
        lmng::transform const &transform) {
          auto &box_mesh = box_collider_meshes.at(entity);
          update_box_uniform(
            frame,
            box_mesh.ubuffer.get(),
            camera,
            lmng::resolve_transform(registry, entity),
            box.extents,
            {1.f, 1.f, 1.f},
            light_direction);
          frame->add({box_mesh.geometry.get()}, viewport);
      });
}

void visual::render_boxes(
  entt::registry const &registry,
  lm::camera const &camera,
  lmgl::iframe *frame,
  lmgl::viewport const &viewport)
{
    registry.view<lmng::box_render const, lmng::transform const>().each(
      [&](
        auto entity,
        lmng::box_render const &box,
        lmng::transform const &transform) {
          auto &box_mesh = box_meshes.at(entity);
          update_box_uniform(
            frame,
            box_mesh.ubuffer.get(),
            camera,
            lmng::resolve_transform(registry, entity),
            box.extents,
            box.colour,
            light_direction);
          frame->add({box_mesh.geometry.get()}, viewport);
      });
}

ivisual_view &visual::set_camera_override(lm::camera const &camera)
{
    camera_override = camera;
    return *this;
}

void visual::add_box_collider_meshes(
  entt::registry const &registry,
  lmgl::irenderer &renderer)
{
    registry.view<lmng::box_collider const>().each(
      [&](entt::entity entity, lmng::box_collider const &box_component) {
          box_collider_meshes.emplace(
            entity, create_box_mesh(&renderer, entity, box_wireframe_material));
      });
}

void visual::recreate(entt::registry const &registry, lmgl::irenderer &renderer)
{
    add_box_meshes(registry, &renderer);
    if (do_render_box_colliders)
        add_box_collider_meshes(registry, renderer);
}

void visual::destroy_box_collider_mesh(
  lmgl::irenderer *renderer,
  entt::entity entity,
  lmgl::resource_sink &resource_sink)
{
    auto &mesh = box_collider_meshes.at(entity);
    resource_sink.add(renderer, mesh.ubuffer);
    resource_sink.add(renderer, mesh.geometry);
    box_collider_meshes.erase(entity);
}

void visual::handle_box_mesh_tag_destroyed(
  entt::registry &registry,
  entt::entity entity)
{
    destroyed_box_meshes.emplace_back(entity);
}

void visual::handle_box_collider_mesh_tag_destroyed(
  entt::registry &registry,
  entt::entity entity)
{
    destroyed_box_collider_meshes.emplace_back(entity);
}
} // namespace lmhuv::internal

namespace lmhuv
{
pvisual_view create_visual_view(visual_view_init const &init)
{
    return std::make_unique<internal::visual>(init);
}

} // namespace lmhuv
