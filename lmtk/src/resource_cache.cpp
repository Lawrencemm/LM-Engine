#include <lmtk/rect_border.h>
#include <lmtk/resource_cache.h>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>

namespace lmtk
{

resource_cache::resource_cache(resource_cache_init const &init)
    : body_font{init.font_loader->create_font(font_init{
        init.renderer,
        init.body_font.typeface_name,
        init.body_font.pixel_size})},
      rect_material{rect::create_material(init.renderer)},
      border_material{rect_border::create_material(init.renderer)},
      text_material{text_layout::create_material(*init.renderer)}
{
}

resource_cache &
  resource_cache::move_resources(lmgl::resource_sink &resource_sink)
{
    resource_sink.add(rect_material, border_material, text_material);
    body_font->move_resources(resource_sink);
    return *this;
}
} // namespace lmtk
