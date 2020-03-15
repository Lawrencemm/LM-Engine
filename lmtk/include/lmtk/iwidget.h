#pragma once

#include <lmgl/lmgl.h>
#include <lmlib/geometry.h>

#include "lmtk.h"

namespace lmtk
{
class iwidget
{
  public:
    virtual lm::size2i get_size() = 0;
    virtual lm::point2i get_position() = 0;
    virtual iwidget &set_rect(lm::point2i position, lm::size2i size) = 0;
    virtual iwidget &move_resources(
      lmgl::irenderer *renderer,
      resource_sink &resource_sink) = 0;
};
} // namespace lmtk
