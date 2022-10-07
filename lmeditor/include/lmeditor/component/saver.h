#pragma once

#include <lmtk/font.h>
#include <lmtk/resource_cache.h>
#include "component.h"

namespace lmeditor
{
class saver_interface : public component_interface
{
  public:
    virtual entt::sink<bool(std::string const &)> on_save() = 0;
    std::vector<command_description> get_command_descriptions() override;
};

struct saver_init
{
    lmgl::irenderer *renderer;
    lmtk::resource_cache const &resource_cache;
    std::string header_text;
    std::string initial_text;

    lm::reference<saver_interface> unique();
};
} // namespace lmeditor
