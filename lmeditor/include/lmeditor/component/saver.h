#pragma once

#include <lmtk/component.h>
#include <lmtk/font.h>

namespace lmeditor
{
class saver_interface : public lmtk::component_interface
{
  public:
    virtual entt::sink<bool(std::string const &)> on_save() = 0;
};

struct saver_init
{
    lmgl::irenderer *renderer;
    lmgl::material text_material;
    lmtk::ifont const *font;
    std::string header_text;
    std::string initial_text;

    lm::reference<saver_interface> unique();
};
} // namespace lmeditor
