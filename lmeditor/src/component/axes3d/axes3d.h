#pragma once

#include <lmgl/lmgl.h>
#include <lmlib/camera.h>

namespace lmeditor
{

struct axes_init;

class axes
{
  public:
    axes(const axes_init &init);
    void on_render(lmgl::iframe *frame, lm::camera const &camera);

    void init_geometry(const axes_init &init);
    void init_rendering(const axes_init &init);

    lmgl::irenderer *renderer;

    std::array<uint16_t, 4> index_data;

    std::array<std::array<float, 3>, 4> positions;
    std::array<uint16_t, 4> indices;
    std::array<std::array<float, 3>, 4> colours;

    lmgl::material material;
    lmgl::buffer ubuffer;
    lmgl::buffer points_vbuffer;
    lmgl::buffer colours_vbuffer;
    lmgl::buffer indices_render_buffer;
    lmgl::geometry render_geometry;
};

struct axes_init
{
    lmgl::irenderer *renderer;
    float extent;
    std::array<float, 3> x_axis_colour;
    std::array<float, 3> z_axis_colour;
    float line_width;
};
} // namespace lmeditor
