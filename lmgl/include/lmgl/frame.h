#pragma once

#include <lmlib/array_proxy.h>
#include <lmlib/geometry.h>

#include "fwd_decl.h"

namespace lmgl
{
using frame_retirement_handler = std::function<void(iframe *)>;

struct viewport
{
    lm::point2f position;
    lm::size2f size;
    float min_depth{0.f}, max_depth{1.f};

    template <typename scalar_type>
    explicit viewport(lm::size2<scalar_type> const &size)
        : position{0, 0}, size{(float)size.width, (float)size.height}
    {
    }

    template <typename scalar_type>
    viewport(
      lm::point2<scalar_type> const &position,
      lm::size2<scalar_type> const &size)
        : position{(float)position.x, (float)position.y},
          size{(float)size.width, (float)size.height}
    {
    }
};

class iframe
{
  public:
    virtual iframe &add(lm::array_proxy<ielement *const> element) = 0;
    virtual iframe &add(
      lm::array_proxy<ielement *const> element,
      struct viewport const &viewport) = 0;

    virtual iframe &clear_colour(std::array<float, 4> colour) = 0;

    virtual iframe &add_buffer_update(
      ibuffer *const buffer,
      lm::array_proxy<const char> data,
      size_t byte_offset) = 0;

    /// Block until the frame's resources are no longer being used to render.
    virtual void wait_complete() = 0;

    virtual lm::size2u size() = 0;

    virtual iframe &build() = 0;
    virtual iframe &submit() = 0;

    virtual ~iframe() = default;
};

template <typename object_type>
void add_buffer_update(
  iframe *frame,
  ibuffer *buffer,
  const object_type &object,
  size_t offset = 0)
{
    frame->add_buffer_update(
      buffer, lm::array_proxy{(char const *)&object, sizeof(object)}, offset);
}
} // namespace lmgl
