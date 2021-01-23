#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <variant>

#include "element.h"
#include "fwd_decl.h"
#include <lmlib/array_proxy.h>
#include <lmlib/geometry.h>

namespace lmgl
{
class igeometry : public virtual ielement
{
  public:
    virtual igeometry &set_n_indices(uint32_t n_indices) = 0;
    virtual igeometry &set_vbuffers(
      lm::array_proxy<ibuffer *const> buffers,
      uint32_t start_index) = 0;
    virtual igeometry &set_index_buffer(ibuffer *buffer) = 0;
    virtual igeometry &set_line_width(float thickness) = 0;

    ~igeometry() override = default;
};

struct geometry_init
{
    lmgl::material material;
    lm::array_proxy<ibuffer *const> const vertex_buffers;
    ibuffer *index_buffer;
    ibuffer *uniform_buffer{nullptr};
    itexture *texture{nullptr};
    float line_width{1.f};
    size_t instance_count{1};
};
} // namespace lmgl
