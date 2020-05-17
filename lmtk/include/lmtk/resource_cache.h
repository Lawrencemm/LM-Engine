#pragma once

#include "font.h"
#include <lmgl/lmgl.h>

namespace lmtk
{
struct resource_cache_init;
class resource_cache
{
  public:
    explicit resource_cache(resource_cache_init const &init);

    resource_cache &move_resources(lmgl::resource_sink &resource_sink);

    font body_font;
    lmgl::material rect_material, border_material, text_material;
};

struct font_description
{
    std::string typeface_name;
    unsigned pixel_size;
};

struct resource_cache_init
{
    lmgl::irenderer *renderer;
    ifont_loader *font_loader;
    font_description body_font;
};
} // namespace lmtk
