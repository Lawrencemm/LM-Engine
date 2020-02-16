#pragma once

#include <memory>

#include <lmlib/reference.h>

namespace lmgl
{
class iwindow;
}

namespace lmgl
{

class camera;
class irenderer;
class ibuffer;
struct buffer_init;
class iframe;
class ielement;
class istage;
struct stage_init;
class igeometry;
struct geometry_init;
class itexture;
struct texture_init;
struct material_init;

using material = void *;

using renderer = lm::reference<irenderer>;
using buffer = lm::reference<ibuffer>;
using frame = lm::reference<iframe>;
using stage = lm::reference<istage>;
using geometry = lm::reference<igeometry>;
using element = lm::reference<ielement>;
using texture = lm::reference<itexture>;
} // namespace lmgl