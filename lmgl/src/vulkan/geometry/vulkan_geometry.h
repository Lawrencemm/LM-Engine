#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

#include <lmlib/rotation_matrix.h>

#include <geometry.h>

#include "../buffer/vulkan_buffer.h"
#include "../vulkan_element.h"

namespace lmgl
{

class vulkan_geometry;
class vulkan_projection;
class vulkan_frame_geometry;
class ProjectionRenderContext;

/// Vulkan geometry object state.
class vulkan_geometry : public virtual igeometry, public vulkan_element
{
  public:
    explicit vulkan_geometry(
      const geometry_init &init,
      vulkan_renderer *renderer);
    lm::reference<vulkan_frame_element>
      create_context_node(vk::Viewport const &viewport) const override;
    vulkan_geometry &set_n_indices(uint32_t n_indices) override;

    void write_descriptors(const geometry_init &init);

    vk::UniqueDescriptorPool descriptor_pool;
    std::vector<vk::DescriptorSet> descriptor_sets;
    float line_width;

    vulkan_material *material;
    std::vector<const vulkan_buffer *> vbuffers;
    lm::reference<vulkan_buffer>::pointer index_buffer;
    size_t n_indices;

    bool hidden{false};
    void create_descriptor_sets(const geometry_init &init);
    igeometry &set_vbuffers(
      lm::array_proxy<ibuffer *const> buffers,
      uint32_t start_index) override;
    igeometry &set_index_buffer(ibuffer *buffer) override;
    igeometry &set_line_width(float width) override;
};

/// Vulkan geometry with finalised state that will render.
class vulkan_frame_geometry : public vulkan_frame_element
{
  public:
    explicit vulkan_frame_geometry(
      const vulkan_geometry &geometry,
      vk::Viewport const &viewport);
    void render(vulkan_frame &context) override;

    std::vector<vk::DescriptorSet> descriptor_sets;

    vk::Pipeline pipeline;
    vk::PipelineLayout pipeline_layout_;
    std::vector<const vulkan_buffer *> vbuffers;
    vulkan_buffer *indices_buffer;
    size_t n_indices;
    vk::IndexType index_type;
    float line_width;
    vk::Viewport viewport;
};
} // namespace lmgl