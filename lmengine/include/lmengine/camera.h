#pragma once

#include "named_type.h"

namespace lmng
{
struct camera
{
    float field_of_view, near_clip, far_clip;
    bool active;
};
} // namespace lmng

LMNG_NAMED_TYPE(lmng::camera, "Camera");
