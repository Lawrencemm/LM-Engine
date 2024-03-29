#include <Resource.h>
#include <lmgl/geometry.h>
#include <lmgl/material.h>
#include <lmtk/shapes.h>
#include "lmlib/variant_visitor.h"

LOAD_RESOURCE(quad_vshader_data, shaders_rect_vert_spv);
LOAD_RESOURCE(quad_pshader_data, shaders_rect_frag_spv);

namespace lmtk
{
struct uniform_buffer
{
    float x, y;
    float width, height;
    std::array<float, 4> colour;
};

lmgl::material rect::create_material(lmgl::irenderer *renderer)
{
    return renderer->create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(quad_vshader_data),
      .pshader_spirv = lm::raw_array_proxy(quad_pshader_data),
      .uniform_size = sizeof(uniform_buffer),
    });
}

rect::rect(
  lmgl::irenderer &renderer,
  lmtk::resource_cache const &resource_cache,
  lm::point2i position,
  lm::size2i size,
  std::array<float, 4> colour)
    : position{position},
      size{size},
      colour{colour},
      ubuffer{renderer.create_buffer(lmgl::buffer_init{
        .usage = lmgl::render_buffer_usage::uniform,
        .data = lm::array_proxy<char const>(nullptr, sizeof(uniform_buffer)),
      })},
      geometry{renderer.create_geometry(lmgl::geometry_init{
        .material = resource_cache.rect_material,
        .uniform_buffer = ubuffer.get(),
      })}
{
    geometry->set_n_indices(6);
}

lm::size2i rect::get_size() { return size; }

lm::point2i rect::get_position() { return position; }

component_interface &rect::set_rect(lm::point2i position, lm::size2i size)
{
    this->position = position;
    this->size = size;
    return *this;
}

rect &rect::move_resources(lmgl::resource_sink &sink)
{
    sink.add(ubuffer);
    return *this;
}
lmtk::component_state rect::handle(const event &event)
{
    event >>
      lm::variant_visitor{
        [&](lmtk::draw_event const &draw_event)
        {
            auto [width, height] = draw_event.frame.size();
            uniform_buffer buf = uniform_buffer{
              2.f * position.x / width - 1,
              2.f * position.y / height - 1,
              2.f * size.width / width,
              2.f * size.height / height,
              colour,
            };
            lmgl::add_buffer_update(&draw_event.frame, ubuffer.get(), buf);
            draw_event.frame.add({geometry.get()});
        },
        [](auto) {},
      };
    return {};
}
} // namespace lmtk
