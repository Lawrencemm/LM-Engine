#pragma once

#include <lmlib/geometry.h>
#include <lmlib/array_proxy.h>

namespace lmgl
{
class itexture
{
  public:
    virtual ~itexture() = default;
};

struct texture_init
{
    lm::size2u dimensions;
    lm::array_proxy<char const> data;
};
} // namespace lmgl
