#include "visual.h"
#include "rendering/shapes.h"
#include <lmhuv.h>
#include <lmhuv/box.h>
#include <lmlib/concat.h>
#include <lmng/camera.h>
#include <lmng/physics.h>

namespace lmhuv::internal
{
struct instance_data
{
    std::array<float, 4> colour;
    Eigen::Matrix4f model;
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
      boxes_ubuffer{create_box_ubuffer(init.renderer)}
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
    std::vector<instance_data> instances_data;
    for (auto [entity, box_render, transform] :
         registry.view<lmng::box_render const, lmng::transform const>().proxy())
    {
        instances_data.emplace_back(instance_data{
          .colour = lm::concat(box_render.colour, 1.f),
          .model = get_box_world_matrix(
            lmng::resolve_transform(registry, entity), box_render.extents)});
    }

    if (!instances_data.empty())
    {
        // Recreate the buffer for per-instance data
        if (box_instances_vbuffer)
        {
            resource_sink.add(box_instances_vbuffer);
        }
        box_instances_vbuffer = renderer->create_buffer(lmgl::buffer_init{
          .usage = lmgl::render_buffer_usage::vertex,
          .data = lm::raw_array_proxy(instances_data),
        });

        if (boxes_geometry)
        {
            resource_sink.add(boxes_geometry);
        }
        // Create a new geometry referencing the new instances data buffer
        boxes_geometry = renderer->create_geometry(lmgl::geometry_init{
          .material = box_material,
          .vertex_buffers =
            {
              box_vpositions.get(),
              box_vnormals.get(),
              box_instances_vbuffer.get(),
            },
          .index_buffer = box_indices.get(),
          .uniform_buffer = boxes_ubuffer.get(),
          .instance_count = instances_data.size(),
        });
        boxes_geometry->set_n_indices(n_box_indices);
    }

    if (!do_render_box_colliders)
        return;

    instances_data.clear();
    for (auto [entity, box_collider, transform] :
         registry.view<lmng::box_collider const, lmng::transform const>()
           .proxy())
    {
        instances_data.emplace_back(instance_data{
          .colour = std::array{1.f, 1.f, 1.f, 1.f},
          .model = get_box_world_matrix(transform, box_collider.extents)});
    }

    if (!instances_data.empty())
    {
        if (box_collider_instances_vbuffer)
        {
            resource_sink.add(box_collider_instances_vbuffer);
        }
        box_collider_instances_vbuffer =
          renderer->create_buffer(lmgl::buffer_init{
            .usage = lmgl::render_buffer_usage::vertex,
            .data = lm::raw_array_proxy(instances_data),
          });

        if (box_colliders_geometry)
        {
            resource_sink.add(box_colliders_geometry);
        }
        box_colliders_geometry = renderer->create_geometry(lmgl::geometry_init{
          .material = box_wireframe_material,
          .vertex_buffers =
            {
              box_vpositions.get(),
              box_vnormals.get(),
              box_collider_instances_vbuffer.get(),
            },
          .index_buffer = box_indices.get(),
          .uniform_buffer = boxes_ubuffer.get(),
          .instance_count = instances_data.size(),
        });
        box_colliders_geometry->set_n_indices(n_box_indices);
    }
}

void visual::add_to_frame(
  entt::registry const &registry,
  lmgl::iframe *frame,
  lmgl::viewport const &viewport)
{
    if (camera_override)
    {
        update_box_uniform(
          frame, boxes_ubuffer.get(), camera_override.value(), light_direction);
        if (boxes_geometry)
        {
            frame->add({boxes_geometry.get()}, viewport);
        }

        if (do_render_box_colliders && box_colliders_geometry)
        {
            frame->add({box_colliders_geometry.get()}, viewport);
        }
        return;
    }

    for (auto [camera_entity, camera_component, transform] :
         registry.view<lmng::camera const, lmng::transform const>().proxy())
    {

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
        update_box_uniform(frame, boxes_ubuffer.get(), camera, light_direction);

        if (boxes_geometry)
        {
            frame->add({boxes_geometry.get()}, viewport);
        }
        if (do_render_box_colliders && box_colliders_geometry)
        {
            frame->add({box_colliders_geometry.get()}, viewport);
        }
    }
}

void visual::move_resources(lmgl::resource_sink &resource_sink)
{
    resource_sink.add(
      box_vpositions,
      box_vnormals,
      box_indices,
      box_material,
      box_wireframe_material,
      boxes_ubuffer,
      box_instances_vbuffer,
      box_collider_instances_vbuffer,
      boxes_geometry,
      box_colliders_geometry);
}

ivisual_view &visual::set_camera_override(lm::camera const &camera)
{
    camera_override = camera;
    return *this;
}

void visual::recreate(entt::registry const &registry, lmgl::irenderer &renderer)
{
}
} // namespace lmhuv::internal

lmhuv::pvisual_view lmhuv::visual_view_init::unique()
{
    return create_visual_view(*this);
}
// namespace lmhuv::internal

namespace lmhuv
{
pvisual_view create_visual_view(visual_view_init const &init)
{
    return std::make_unique<internal::visual>(init);
}

} // namespace lmhuv
