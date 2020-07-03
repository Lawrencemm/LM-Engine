#pragma once

#include "component.h"
#include <filesystem>

namespace lmeditor
{
struct asset_list_init
{
    std::filesystem::path asset_dir;
    lm::point2i position{0, 0};
    lm::size2i size{0, 0};
    lmgl::irenderer *renderer;
    lmtk::resource_cache const &resource_cache;

    component operator()();
};
} // namespace lmeditor
