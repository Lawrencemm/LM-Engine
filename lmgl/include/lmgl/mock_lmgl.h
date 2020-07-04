#pragma once

#include <lmgl/geometry.h>
#include <lmgl/lmgl.h>
#include <lmgl/renderer.h>
#include <lmgl/stage.h>
#include <lmgl/texture.h>

namespace lmgl
{

class MockFrame : public iframe
{
  public:
    iframe &add(lm::array_proxy<ielement *const> elements) override
    {
        return *this;
    }
    iframe &add(
      lm::array_proxy<ielement *const> elements,
      struct viewport const &viewport) override
    {
        return *this;
    }
    iframe &clear_colour(std::array<float, 4> colour) override {}
    iframe &add_buffer_update(
      ibuffer *const buffer,
      lm::array_proxy<const char> data,
      size_t byte_offset) override
    {
        return *this;
    }
    void wait_complete() override {}
    lm::size2u size() override { return lm::size2u(); }
    iframe &build() override { return *this; }
    iframe &submit() override { return *this; }
};

class MockStage : public istage
{
  public:
    ~MockStage() override = default;
    frame wait_for_frame() override { return std::make_unique<MockFrame>(); }

    irenderer *renderer;
};

class MockBuffer : public ibuffer
{
  public:
};

class MockGeometry : public igeometry
{
  public:
    igeometry &set_n_indices(uint32_t n_indices) override { return *this; }
    igeometry &set_vbuffers(
      lm::array_proxy<ibuffer *const> buffers,
      uint32_t start_index) override
    {
        return *this;
    }
    igeometry &set_index_buffer(ibuffer *buffer) override { return *this; }
    igeometry &set_line_width(float thickness) override { return *this; }
};

class MockTexture : public itexture
{
  public:
};

class MockRenderer : public irenderer
{
  public:
    stage create_stage(const stage_init &init) override
    {
        return std::make_unique<MockStage>();
    }
    material create_material(material_init const &init) override { return 0; }
    buffer create_buffer(const buffer_init &init) override
    {
        return std::make_unique<MockBuffer>();
    }
    ~MockRenderer() override = default;
    geometry create_geometry(const geometry_init &init) override
    {
        return std::make_unique<MockGeometry>();
    }
    texture create_texture(const texture_init &init) override
    {
        return std::make_unique<MockTexture>();
    }
    irenderer &destroy_material(material material1) override { return *this; }
    stage create_texture_stage(itexture *itexture) override
    {
        return lmgl::stage();
    }
};
} // namespace lmgl
