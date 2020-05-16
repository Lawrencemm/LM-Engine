#pragma once

#include "input_event.h"
#include "resource_sink.h"

#include <lmgl/frame.h>
#include <lmgl/renderer.h>
#include <lmlib/geometry.h>

#include "widget.h"

namespace lmtk
{
struct box_init;
class rect : public widget_interface
{
  public:
    static lmgl::material create_material(lmgl::irenderer *renderer);

    explicit rect(
      lmgl::irenderer &renderer,
      lmgl::material material,
      lm::point2i position,
      lm::size2i size,
      std::array<float, 4> colour);

    lm::size2i get_size() override;
    lm::point2i get_position() override;

    widget_interface &set_rect(lm::point2i position, lm::size2i size) override;

    rect &add_to_frame(lmgl::iframe *frame);

    rect &
      move_resources(lmgl::irenderer *renderer, resource_sink &sink) override;

  private:
    lm::point2i position;
    lm::size2i size;
    std::array<float, 4> colour;
    lmgl::buffer ubuffer;
    lmgl::geometry geometry;
};
} // namespace lmtk
