#include "font.h"

#include <freetype/ftglyph.h>
#include <lmgl/lmgl.h>
#include <lmlib/enumerate.h>
#include <lmlib/range.h>

namespace lmtk
{
font_internal::font_internal(font_atlas atlas) : atlas{std::move(atlas)} {
}

constexpr unsigned first_char = 32;
constexpr unsigned end_char = 127;
lm::range char_range{first_char, end_char};
constexpr size_t n_glyphs = end_char - first_char;

font_atlas
  font_internal::create_font_atlas(lmgl::irenderer &renderer, FT_Face face)
{
    font_atlas atlas{};

    std::array<FT_BitmapGlyph, n_glyphs> stored_glyphs;

    atlas.width = atlas.height = 0;

    for (auto [zero_based, i] : lm::enumerate(char_range))
    {
        if (FT_Load_Char(face, i, FT_LOAD_DEFAULT))
        {
            continue;
        }
        FT_Glyph *glyph_ptr = (FT_Glyph *)&stored_glyphs[zero_based];

        FT_Get_Glyph(face->glyph, glyph_ptr);
        FT_Glyph_To_Bitmap(glyph_ptr, FT_RENDER_MODE_NORMAL, nullptr, 1);

        auto bmp_glyph = stored_glyphs[zero_based];

        atlas.metrics.emplace_back(glyph_metrics{
          .advance = {face->glyph->advance.x >> 6, face->glyph->advance.y >> 6},
          .size = {face->glyph->bitmap.width, face->glyph->bitmap.rows},
          .topleft_pos = {face->glyph->bitmap_left, face->glyph->bitmap_top},
          .texture_x = atlas.width,
        });

        atlas.width += bmp_glyph->bitmap.width;
        atlas.height = std::max(atlas.height, bmp_glyph->bitmap.rows);
    }

    std::vector<unsigned char> atlas_memory(atlas.width * atlas.height, 0);

    unsigned texture_pos_x = 0;
    for (auto [i, glyph] : lm::enumerate(stored_glyphs))
    {
        for (auto row : lm::range(glyph->bitmap.rows))
        {
            memcpy(
              atlas_memory.data() + atlas.width * row + texture_pos_x,
              glyph->bitmap.buffer + row * glyph->bitmap.pitch,
              glyph->bitmap.width);
        }
        texture_pos_x += glyph->bitmap.width;
    }

    atlas.texture = renderer.create_texture(lmgl::texture_init{
      .dimensions = lm::size2u{atlas.width, atlas.height},
      .data = lm::raw_array_proxy(atlas_memory),
    });

    for (auto bitmapglyph : stored_glyphs)
    {
        FT_Done_Glyph((FT_Glyph)bitmapglyph);
    }

    return std::move(atlas);
}

unsigned int font_internal::get_max_glyph_height() const
{
    return atlas.height;
}

lmgl::itexture *font_internal::get_texture() const
{
    return atlas.texture.get();
}

glyph_metrics const &font_internal::get_metrics(char c) const
{
    return atlas.metrics[c - first_char];
}

lm::size2i font_internal::get_texture_size() const
{
    return {static_cast<int>(atlas.width), static_cast<int>(atlas.height)};
}

ifont &font_internal::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    resource_sink.add(atlas.texture);
    return *this;
}

} // namespace lmtk
