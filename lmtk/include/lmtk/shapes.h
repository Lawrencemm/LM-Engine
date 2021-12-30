#pragma once

#include "event.h"
#include "lmgl/resource_sink.h"

#include <lmgl/frame.h>
#include <lmgl/renderer.h>
#include <lmlib/geometry.h>

#include "resource_cache.h"
#include "component.h"

namespace lmtk
{
class rect : public component_interface
{
  public:
    static lmgl::material create_material(lmgl::irenderer *renderer);
    bool handle(const event &event) override;

    explicit rect(
      lmgl::irenderer &renderer,
      lmtk::resource_cache const &resource_cache,
      lm::point2i position,
      lm::size2i size,
      std::array<float, 4> colour);

    lm::size2i get_size() override;
    lm::point2i get_position() override;

    component_interface &
      set_rect(lm::point2i position, lm::size2i size) override;

    rect &move_resources(lmgl::resource_sink &sink) override;

  private:
    lm::point2i position;
    lm::size2i size;
    std::array<float, 4> colour;
    lmgl::buffer ubuffer;
    lmgl::geometry geometry;
};
} // namespace lmtk
