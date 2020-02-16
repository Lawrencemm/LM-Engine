#pragma once

#include <lmpl/lmpl.h>

#include "fwd_decl.h"

namespace lmgl
{
class istage
{
  public:
    virtual frame wait_for_frame() = 0;
    virtual ~istage() = default;
};

struct stage_init
{
    lmpl::iwindow *output_window;
};
} // namespace lmgl
