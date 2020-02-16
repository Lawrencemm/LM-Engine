#pragma once

#include <fmt/format.h>
#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <lmtk/font.h>

namespace lmtk
{
class font_loader_internal : public ifont_loader
{
  public:
    font_loader_internal();

    ~font_loader_internal() override;
    font create_font(font_init const &init) override;

    FT_Library ft_library;
    FcConfig *font_config;
    std::string find_font_file(const char *typeface_name) const;
};
} // namespace lmtk
