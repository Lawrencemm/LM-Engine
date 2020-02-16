#pragma once

#include <lmgl/fwd_decl.h>
#include <lmlib/geometry.h>
#include <lmtk/font.h>

namespace lmtk
{
struct font_atlas
{
    lmgl::texture texture;
    std::vector<glyph_metrics> metrics;
    unsigned width, height;
};

class font_internal : public ifont
{
  public:
    font_internal(font_atlas atlas);
    lm::size2i get_texture_size() const override;

    static font_atlas
      create_font_atlas(lmgl::irenderer &renderer, FT_Face face);

    unsigned int get_max_glyph_height() const override;

    lmgl::itexture *get_texture() const override;

    glyph_metrics const &get_metrics(char c) const override;
    ifont &move_resources(
      lmgl::irenderer *renderer,
      resource_sink &resource_sink) override;

    font_atlas atlas;
};
} // namespace lmtk
