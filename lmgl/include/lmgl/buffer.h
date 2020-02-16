#pragma once

#include <variant>

#include <lmlib/array_proxy.h>
#include <lmlib/array_size.h>

namespace lmgl
{
class ibuffer
{
  public:
    virtual ~ibuffer() = default;
};

enum render_buffer_usage
{
    vertex,
    index,
    uniform
};

struct buffer_init
{
    render_buffer_usage usage;
    lm::array_proxy<char const> data;
};
} // namespace lmgl
