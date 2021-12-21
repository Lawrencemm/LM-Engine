#include "font.h"

#include <codecvt>
#include <locale>
#include <string>

#include <dwrite.h>
#include <fmt/format.h>
#include <lmpl/windows_error.h>
#include <scope_guard.hpp>

class font_loader_internal : public lmtk::ifont_loader
{
  public:
    font_loader_internal();
    lmtk::font create_font(lmtk::font_init const &init) override;

  private:
    IDWriteFactory *pDWriteFactory_;
    FT_Library ft_library;
};

font_loader_internal::font_loader_internal()
{
    auto hr = DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED,
      __uuidof(IDWriteFactory),
      reinterpret_cast<IUnknown **>(&pDWriteFactory_));
    if (!SUCCEEDED(hr))
    {
        throw std::runtime_error{"Couldn't instantiate Directwrite Factory."};
    }

    auto error = FT_Init_FreeType(&ft_library);
    if (error)
        throw std::runtime_error{"Freeetype library initialisation failed."};
}

lmtk::font font_loader_internal::create_font(lmtk::font_init const &init)
{
    IDWriteTextFormat *pTextFormat_;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(init.typeface_name.c_str());

    lmpl::throw_failed_hr(pDWriteFactory_->CreateTextFormat(
      wide.c_str(),
      NULL,
      DWRITE_FONT_WEIGHT_REGULAR,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      (FLOAT)init.pixel_size,
      L"en-us",
      &pTextFormat_));

    auto format_releaser =
      sg::make_scope_guard([&]() { pTextFormat_->Release(); });

    auto font_family_name_array_length{
      pTextFormat_->GetFontFamilyNameLength() + 1,
    };

    auto family_name_string =
      std::make_unique<wchar_t[]>(font_family_name_array_length);

    lmpl::throw_failed_hr(pTextFormat_->GetFontFamilyName(
      family_name_string.get(), font_family_name_array_length));

    IDWriteFontCollection *font_collection;
    lmpl::throw_failed_hr(pTextFormat_->GetFontCollection(&font_collection));

    auto collection_releaser =
      sg::make_scope_guard([&]() { font_collection->Release(); });

    UINT32 family_index;
    BOOL family_exists;
    lmpl::throw_failed_hr(font_collection->FindFamilyName(
      family_name_string.get(), &family_index, &family_exists));

    IDWriteFontFamily *font_family;
    lmpl::throw_failed_hr(
      font_collection->GetFontFamily(family_index, &font_family));

    auto family_releaser =
      sg::make_scope_guard([&]() { font_family->Release(); });

    IDWriteFont *font;
    lmpl::throw_failed_hr(font_family->GetFirstMatchingFont(
      DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      DWRITE_FONT_STYLE_NORMAL,
      &font));

    auto font_releaser = sg::make_scope_guard([&]() { font->Release(); });

    IDWriteFontFace *font_face;
    lmpl::throw_failed_hr(font->CreateFontFace(&font_face));

    auto font_face_releaser =
      sg::make_scope_guard([&]() { font_face->Release(); });

    UINT32 n_font_face_files;
    lmpl::throw_failed_hr(font_face->GetFiles(&n_font_face_files, nullptr));

    if (n_font_face_files > 1)
        throw std::runtime_error{"More than one font file for font."};

    auto font_files = std::make_unique<IDWriteFontFile *[]>(n_font_face_files);

    lmpl::throw_failed_hr(
      font_face->GetFiles(&n_font_face_files, font_files.get()));

    auto font_files_releaser = sg::make_scope_guard([&]() {
        for (auto font_file_index : lm::range(n_font_face_files))
            font_files[font_file_index]->Release();
    });

    IDWriteFontFile *font_file{font_files[0]};

    IDWriteFontFileLoader *font_file_loader;

    lmpl::throw_failed_hr(font_file->GetLoader(&font_file_loader));

    auto loader_releaser =
      sg::make_scope_guard([&]() { font_file_loader->Release(); });

    void const *font_file_reference_key;
    UINT32 reference_key_size;
    lmpl::throw_failed_hr(font_file->GetReferenceKey(
      &font_file_reference_key, &reference_key_size));

    IDWriteFontFileStream *font_file_stream;
    lmpl::throw_failed_hr(font_file_loader->CreateStreamFromKey(
      font_file_reference_key, reference_key_size, &font_file_stream));

    auto stream_releaser =
      sg::make_scope_guard([&]() { font_file_stream->Release(); });

    UINT64 font_file_size;
    lmpl::throw_failed_hr(font_file_stream->GetFileSize(&font_file_size));

    void const *fragment_start;
    void *fragment_context;
    lmpl::throw_failed_hr(font_file_stream->ReadFileFragment(
      &fragment_start, 0, font_file_size, &fragment_context));

    auto fragment_releaser = sg::make_scope_guard(
      [&]() { font_file_stream->ReleaseFileFragment(fragment_context); });

    FT_Face freetype_font_face;

    auto error = FT_New_Memory_Face(
      ft_library,
      (FT_Byte const *)fragment_start,
      (FT_Long)font_file_size,
      0,
      &freetype_font_face);

    if (error)
    {
        throw std::runtime_error(
          fmt::format("Couldn't load {} with freetype.", init.typeface_name));
    }

    auto ft_face_releaser =
      sg::make_scope_guard([&]() { FT_Done_Face(freetype_font_face); });

    FT_Set_Pixel_Sizes(freetype_font_face, 0, init.pixel_size);

    return std::make_unique<lmtk::font_internal>(
      lmtk::font_internal::create_font_atlas(
        *init.renderer, freetype_font_face));
}

namespace lmtk
{
font_loader create_font_loader()
{
    return std::make_unique<font_loader_internal>();
}
} // namespace lmtk
