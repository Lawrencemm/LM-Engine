#include <Resource.h>

#include <lmgl/material.h>
#include <lmtk/rect_border.h>
#include "lmlib/variant_visitor.h"

LOAD_RESOURCE(vshader_spirv, shaders_rect_border_vert_spv);
LOAD_RESOURCE(pshader_spirv, shaders_rect_border_frag_spv);

namespace lmtk
{
struct rect_uniform_buffer
{
    float xpos, ypos, width, height;
    std::array<float, 4> colour;
    float thickness, xscale, yscale;
};

lmgl::material rect_border::create_material(lmgl::irenderer *renderer)
{
    return renderer->create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(vshader_spirv),
      .pshader_spirv = lm::raw_array_proxy(pshader_spirv),
      .topology = lmgl::topology::line_list,
      .uniform_size = sizeof(rect_uniform_buffer),
      .do_depth_test = false,
    });
}

rect_border::rect_border(
  lmgl::irenderer *renderer,
  resource_cache const &resource_cache,
  lm::point2i position,
  lm::size2i size,
  std::array<float, 4> colour,
  float border_thickness)
    : position{position},
      size{size},
      thickness{border_thickness},
      colour{colour},
      ubuffer{renderer->create_buffer(lmgl::buffer_init{
        .usage = lmgl::render_buffer_usage::uniform,
        .data =
          lm::array_proxy<char const>(nullptr, sizeof(rect_uniform_buffer)),
      })},
      geometry{renderer->create_geometry(lmgl::geometry_init{
        .material = resource_cache.border_material,
        .uniform_buffer = ubuffer.get(),
        .line_width = border_thickness,
      })}
{
    geometry->set_n_indices(8);
}

void rect_border::set_thickness(float thickness)
{
    this->thickness = thickness;
    geometry->set_line_width(thickness);
}

lm::size2i rect_border::get_size() { return size; }
lm::point2i rect_border::get_position() { return position; }
bool rect_border::handle(const event &event)
{
    event >>
      lm::variant_visitor{
        [&](lmtk::draw_event const &draw_event)
        {
            auto [width, height] = draw_event.frame.size();
            Eigen::Vector4f transform;
            rect_uniform_buffer buf{
              (float)position.x,
              (float)position.y,
              (float)size.width,
              (float)size.height,
              colour,
              (float)thickness,
              2.f / width,
              2.f / height,
            };
            lmgl::add_buffer_update(&draw_event.frame, ubuffer.get(), buf);
            draw_event.frame.add({geometry.get()});
        },
        [](auto) {},
      };
    return false;
}
} // namespace lmtk
