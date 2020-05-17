#pragma once

#include "lmgl/resource_sink.h"

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <lmgl/fwd_decl.h>
#include <lmlib/geometry.h>
#include <lmlib/reference.h>

namespace lmtk
{
struct glyph_metrics
{
    lm::point2l advance;
    lm::size2u size;
    lm::point2i topleft_pos;
    unsigned texture_x;
};

class ifont
{
  public:
    virtual unsigned get_max_glyph_height() const = 0;
    virtual lmgl::itexture *get_texture() const = 0;
    virtual lm::size2i get_texture_size() const = 0;
    virtual glyph_metrics const &get_metrics(char c) const = 0;
    virtual ifont &move_resources(lmgl::resource_sink &resource_sink) = 0;
    virtual ~ifont() = default;
};

struct font_init
{
    lmgl::irenderer *renderer;
    std::string const &typeface_name;
    unsigned pixel_size;
};

using font = lm::reference<ifont>;

class ifont_loader
{
  public:
    virtual font create_font(font_init const &init) = 0;
    virtual ~ifont_loader() = default;
};

using font_loader = lm::reference<ifont_loader>;

font_loader create_font_loader();
} // namespace lmtk
