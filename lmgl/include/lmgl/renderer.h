#pragma once

#include <source_location>

#include "buffer.h"
#include "fwd_decl.h"

namespace lmgl
{
class irenderer
{
  public:
    virtual stage create_stage(const stage_init &init) = 0;
    virtual geometry create_geometry(const geometry_init &init) = 0;
    virtual buffer create_buffer(const buffer_init &init) = 0;
    virtual texture create_texture(const texture_init &init) = 0;
    virtual material create_material(material_init const &init, std::source_location location = std::source_location::current()) = 0;
    virtual irenderer &destroy_material(material) = 0;
    virtual stage create_texture_stage(itexture *) = 0;
    virtual ~irenderer() = default;
};

struct renderer_init
{
};

renderer create_renderer(const renderer_init &init);
} // namespace lmgl
