#include <lmlib/enumerate.h>
#include <lmlib/range.h>

#include "../vulkan_renderer.h"
#include "../vulkan_texture.h"
#include "../vulkan_window_stage.h"
#include "vulkan_geometry.h"

namespace lmgl
{
geometry vulkan_renderer::create_geometry(const geometry_init &init)
{
    return std::make_unique<vulkan_geometry>(init, this);
}

vulkan_geometry::vulkan_geometry(
  const geometry_init &init,
  vulkan_renderer *renderer)
    : vulkan_element{renderer},
      material{reinterpret_cast<vulkan_material *>(init.material)},
      index_buffer{dynamic_cast<vulkan_buffer *>(init.index_buffer)},
      line_width{init.line_width},
      instance_count{init.instance_count}
{
    for (auto &buffer : init.vertex_buffers)
    {
        vbuffers.emplace_back(dynamic_cast<vulkan_buffer *>(buffer));
    }

    create_descriptor_sets(init);
    write_descriptors(init);
}

void vulkan_geometry::create_descriptor_sets(const geometry_init &init)
{
    auto pool_sizes = std::array{
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1},
      vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1},
    };

    auto pool_create_info = vk::DescriptorPoolCreateInfo{}
                              .setPPoolSizes(pool_sizes.data())
                              .setPoolSizeCount((uint32_t)pool_sizes.size())
                              .setMaxSets(1);

    auto &vk_device = renderer->vk_device;

    descriptor_pool = vk_device->createDescriptorPoolUnique(pool_create_info);

    auto alloc_info = vk::DescriptorSetAllocateInfo{}
                        .setPSetLayouts(&*material->descriptor_set_layout)
                        .setDescriptorPool(*descriptor_pool)
                        .setDescriptorSetCount(1);

    descriptor_sets = vk_device->allocateDescriptorSets(alloc_info);
} // namespace lmgl

void vulkan_geometry::write_descriptors(const geometry_init &init)
{
    std::vector<vk::WriteDescriptorSet> write_descriptor_sets;

    vk::DescriptorBufferInfo cutom_buf_info;
    if (init.uniform_buffer)
    {
        auto vk_buffer = dynamic_cast<vulkan_buffer *>(init.uniform_buffer);
        cutom_buf_info.setBuffer(vk_buffer->buffer.get())
          .setRange(VK_WHOLE_SIZE);

        write_descriptor_sets.push_back(
          vk::WriteDescriptorSet{}
            .setDstBinding(0)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstSet(descriptor_sets[0])
            .setPBufferInfo(&cutom_buf_info));
    }

    vk::DescriptorImageInfo storage_image_info;
    if (init.texture)
    {
        auto vk_texture = (vulkan_texture *)init.texture;

        storage_image_info.setSampler(vk_texture->sampler.get())
          .setImageView(vk_texture->view.get())
          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        write_descriptor_sets.emplace_back(vk::WriteDescriptorSet{
          descriptor_sets[0],
          1,
          0,
          1,
          vk::DescriptorType::eCombinedImageSampler,
          &storage_image_info,
        });
    }

    auto &vk_device = renderer->vk_device;

    vk_device->updateDescriptorSets(write_descriptor_sets, {});
}

vulkan_frame_geometry::vulkan_frame_geometry(
  const vulkan_geometry &geometry,
  vk::Viewport const &viewport,
  vk::Rect2D const &scissor)
    : descriptor_sets{geometry.descriptor_sets},
      pipeline{
        reinterpret_cast<vulkan_material *>(geometry.material)->pipeline.get()},
      pipeline_layout_{reinterpret_cast<vulkan_material *>(geometry.material)
                         ->pipeline_layout.get()},
      vbuffers{geometry.vbuffers},
      indices_buffer{geometry.index_buffer},
      index_type{
        reinterpret_cast<vulkan_material *>(geometry.material)->index_type},
      n_indices{geometry.n_indices},
      instance_count{geometry.instance_count},
      line_width{geometry.line_width},
      viewport{viewport},
      scissor{scissor}
{
}

lm::reference<vulkan_frame_element> vulkan_geometry::create_context_node(
  vk::Viewport const &viewport,
  vk::Rect2D const &scissor) const
{
    return std::make_unique<vulkan_frame_geometry>(*this, viewport, scissor);
}

void vulkan_frame_geometry::render(vulkan_frame &context)
{
    auto &command_buffer = context.command_buffer;

    command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

    for (auto const &[i, buffer] : lm::enumerate(vbuffers))
    {
        command_buffer->bindVertexBuffers(
          (uint32_t)i, {buffer->buffer.get()}, {0});
    }

    if (indices_buffer)
        command_buffer->bindIndexBuffer(
          indices_buffer->buffer.get(), 0, index_type);

    command_buffer->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_layout_,
      0,
      {descriptor_sets},
      {});

    command_buffer->setLineWidth(line_width);

    command_buffer->setViewport(0, 1, &viewport);
    command_buffer->setScissor(0, 1, &scissor);

    if (indices_buffer)
        command_buffer->drawIndexed(
          (uint32_t)n_indices, (uint32_t)instance_count, 0, 0, 0);
    else
        command_buffer->draw(
          (uint32_t)n_indices, (uint32_t)instance_count, 0, 0);
}

vulkan_geometry &vulkan_geometry::set_n_indices(uint32_t n_indices)
{
    this->n_indices = n_indices;
    return *this;
}

igeometry &vulkan_geometry::set_vbuffers(
  lm::array_proxy<ibuffer *const> buffers,
  uint32_t start_index)
{
    assert(vbuffers.size() - start_index >= buffers.size());
    for (const auto &[i, buffer] : enumerate(buffers))
    {
        vbuffers[start_index + i] = dynamic_cast<vulkan_buffer *>(buffer);
    }
    return *this;
}

igeometry &vulkan_geometry::set_index_buffer(ibuffer *buffer)
{
    index_buffer = dynamic_cast<vulkan_buffer *>(buffer);
    return *this;
}

igeometry &vulkan_geometry::set_line_width(float width)
{
    line_width = width;
    return *this;
}
} // namespace lmgl
