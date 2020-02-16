#include "font_loader.h"

#include <scope_guard.hpp>

#include "../font.h"

namespace lmtk
{
font font_loader_internal::create_font(font_init const &init)
{
    auto font_file_path = find_font_file(init.typeface_name.c_str());

    FT_Face freetype_font_face;

    auto error =
      FT_New_Face(ft_library, font_file_path.c_str(), 0, &freetype_font_face);

    if (error)
    {
        throw std::runtime_error(
          fmt::format("Couldn't load {} with freetype.", font_file_path));
    }

    auto ft_face_releaser =
      sg::make_scope_guard([&]() { FT_Done_Face(freetype_font_face); });

    FT_Set_Pixel_Sizes(freetype_font_face, 0, init.pixel_size);

    return std::make_unique<lmtk::font_internal>(
      lmtk::font_internal::create_font_atlas(
        *init.renderer, freetype_font_face));
}

font_loader_internal::font_loader_internal()
{
    auto error = FT_Init_FreeType(&ft_library);
    if (error)
        throw std::runtime_error{"Freeetype library initialisation failed."};

    font_config = FcInitLoadConfigAndFonts();
}

font_loader_internal::~font_loader_internal()
{
    FT_Done_FreeType(ft_library);
    FcConfigDestroy(font_config);
}

std::string
  font_loader_internal::find_font_file(const char *typeface_name) const
{
    // make pattern from font name
    FcPattern *pat = FcNameParse((const FcChar8 *)typeface_name);
    FcConfigSubstitute(font_config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);
    auto destroy_pat = sg::make_scope_guard([pat] { FcPatternDestroy(pat); });

    std::string font_file_name;

    // find the font
    FcResult result;
    FcPattern *font = FcFontMatch(font_config, pat, &result);
    if (!font)
    {
        throw std::runtime_error{
          fmt::format("Font \"{}\" not found.", typeface_name)};
    }
    auto destroy_font =
      sg::make_scope_guard([font] { FcPatternDestroy(font); });
    FcChar8 *file = NULL;
    if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
    {
        // This might be a fallback font.
        font_file_name = std::string{(char *)file};
    }
    else
    {
        throw std::runtime_error{
          fmt::format("Font \"{}\" not found.", typeface_name)};
    }

    return font_file_name;
}

font_loader create_font_loader()
{
    return std::make_unique<font_loader_internal>();
}
} // namespace lmtk
