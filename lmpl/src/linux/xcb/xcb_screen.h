#pragma once

#include <lmlib/geometry.h>

namespace lmpl
{

class XcbScreen : public iscreen
{
  public:
    lm::size2i get_size() override { return {width, height}; };

    uint16_t width, height;
};
} // namespace lmpl
