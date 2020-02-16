#pragma once

#include <lmengine/named_type.h>

struct protagonist_component
{
    float move_force{10.f}, jump_impulse{10.f};
};

LMNG_NAMED_TYPE(protagonist_component, "Protagonist");
