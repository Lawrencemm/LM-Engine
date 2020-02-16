#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <Resource.h>

#include <fmt/ostream.h>

#include <lmgl/geometry.h>
#include <lmgl/material.h>
#include <lmgl/renderer.h>
#include <lmgl/texture.h>
#include <lmlib/enumerate.h>
#include <lmlib/range.h>
#include <lmtk/text_layout.h>

LOAD_RESOURCE(vshader_data, shaders_text_vert_spv);
LOAD_RESOURCE(pshader_data, shaders_text_frag_spv);

namespace lmtk
{
struct glyph_metrics;

struct text_render_uniform
{
    Eigen::Matrix4f transform;
    std::array<float, 3> colour;
};

lmgl::material text_layout::create_material(lmgl::irenderer &renderer)
{
    return renderer.create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(vshader_data),
      .pshader_spirv = lm::raw_array_proxy(pshader_data),
      .index_type = lmgl::index_type::two_bytes,
      .vertex_input_types =
        {
          lmgl::input_type::float_2,
          lmgl::input_type::float_2,
        },
      .uniform_size = sizeof(text_render_uniform),
      .texture = true,
      .do_depth_test = false,
    });
}

text_layout::text_layout(text_layout_init const &init)
    : material{init.material},
      alignment{init.alignment},
      position{init.position},
      colour{init.colour},
      ubuffer{init.renderer.create_buffer(lmgl::buffer_init{
        .usage = lmgl::render_buffer_usage::uniform,
        .data =
          lm::array_proxy<char const>{nullptr, sizeof(text_render_uniform)},
      })},
      max_glyph_height{init.font->get_max_glyph_height()}
{
    if (!init.text.empty() && init.font->get_texture())
    {
        recreate(init.renderer, init.font, init.text);
    }
}

text_layout &text_layout::render(lmgl::iframe *frame)
{
    if (vposbuffer == nullptr)
        return *this;

    Eigen::Matrix3f transform = Eigen::Matrix3f::Identity();
    const lm::size2u &frame_size = frame->size();
    transform(0, 0) = 2.f / frame_size.width;
    transform(1, 1) = -2.f / frame_size.height;

    Eigen::Vector2f fpos = {-1.f, -1.f};
    fpos += Eigen::Vector2f{
      (float)2 * position.x / frame_size.width,
      (float)2 * (position.y + max_glyph_height) / frame_size.height,
    };

    transform.col(2).head<2>() = fpos;

    if (alignment == text_alignment::center)
    {
        auto width = 2 * pixel_width / frame_size.width;
        transform(0, 2) -= width / 2;
    }

    Eigen::Matrix4f padded_transform = Eigen::Matrix4f::Identity();
    padded_transform.block(0, 0, 3, 3) = transform;
    text_render_uniform uniform{padded_transform, colour};

    lmgl::add_buffer_update(frame, ubuffer.get(), uniform);
    frame->add({geometry_.get()});
    return *this;
}

void text_layout::move_text_resources(
  resource_sink &sink,
  lmgl::irenderer *renderer)
{
    sink.add(renderer, vposbuffer, tcoordbuffer, ibuffer, geometry_);
}

void text_layout::move_resources(lmgl::irenderer *renderer, resource_sink &sink)
{
    move_text_resources(sink, renderer);
    sink.add(renderer, ubuffer);
}

void text_layout::recreate(
  lmgl::irenderer &renderer,
  ifont const *font,
  std::string const &text)
{
    if (!font->get_texture())
        return;

    std::vector<Eigen::Vector2f> vertices;
    std::vector<Eigen::Vector2f> tcoords;
    std::vector<unsigned short> indices;

    float hpos{0.f};
    for (auto [i, c] : lm::enumerate(text))
    {
        auto char_metrics = font->get_metrics(c);

        float x0 = char_metrics.topleft_pos.x + hpos;
        float x1 = x0 + char_metrics.size.width;
        float y0 = char_metrics.topleft_pos.y;
        float y1 = y0 - char_metrics.size.height;

        vertices.emplace_back(Eigen::Vector2f{x0, y0});
        vertices.emplace_back(Eigen::Vector2f{x0, y1});
        vertices.emplace_back(Eigen::Vector2f{x1, y1});
        vertices.emplace_back(Eigen::Vector2f{x1, y0});

        auto texture_size = font->get_texture_size();

        float tx0 = float(char_metrics.texture_x) / texture_size.width;
        float tx1 = float(char_metrics.texture_x + char_metrics.size.width) /
                    texture_size.width;
        float ty0 = 0.f;
        float ty1 = float(char_metrics.size.height) / texture_size.height;

        tcoords.emplace_back(Eigen::Vector2f{tx0, ty0});
        tcoords.emplace_back(Eigen::Vector2f{tx0, ty1});
        tcoords.emplace_back(Eigen::Vector2f{tx1, ty1});
        tcoords.emplace_back(Eigen::Vector2f{tx1, ty0});

        unsigned short first_v = i * 4;

        indices.emplace_back(first_v);
        indices.emplace_back(first_v + 1);
        indices.emplace_back(first_v + 2);
        indices.emplace_back(first_v + 2);
        indices.emplace_back(first_v + 3);
        indices.emplace_back(first_v);

        hpos += char_metrics.advance.x;
    }

    pixel_width = hpos;

    vposbuffer = renderer.create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_usage::vertex,
      .data = lm::raw_array_proxy(vertices),
    });
    tcoordbuffer = renderer.create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_usage::vertex,
      .data = lm::raw_array_proxy(tcoords),
    });
    ibuffer = renderer.create_buffer(lmgl::buffer_init{
      .usage = lmgl::index,
      .data = lm::raw_array_proxy(indices),
    });

    geometry_ = renderer.create_geometry(lmgl::geometry_init{
      .material = material,
      .vertex_buffers =
        {
          vposbuffer.get(),
          tcoordbuffer.get(),
        },
      .index_buffer = ibuffer.get(),
      .uniform_buffer = ubuffer.get(),
      .texture = font->get_texture(),
    });

    geometry_->set_n_indices((uint32_t)indices.size());
}

void text_layout::set_text(
  lmgl::irenderer &renderer,
  ifont const *font,
  std::string const &text,
  resource_sink &sink)
{
    move_text_resources(sink, &renderer);

    if (text.empty())
        return;

    recreate(renderer, font, text);
}

lm::size2i text_layout::get_size()
{
    return {(int)pixel_width, (int)max_glyph_height};
}
} // namespace lmtk
