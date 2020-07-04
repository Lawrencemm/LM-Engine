#pragma once

#include "component.h"
#include <entt/signal/sigh.hpp>
#include <filesystem>

namespace lmeditor
{
class asset_list_interface : public component_interface
{
  public:
    virtual entt::sink<bool(std::filesystem::path const &)> on_select_map() = 0;
};

struct asset_list_init
{
    std::filesystem::path asset_dir;
    lm::point2i position{0, 0};
    lm::size2i size{0, 0};
    lmgl::irenderer *renderer;
    lmtk::resource_cache const &resource_cache;

    std::unique_ptr<asset_list_interface> operator()();
};
} // namespace lmeditor
