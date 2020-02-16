#include "vulkan_buffer_base.h"
#include "../vulkan_renderer.h"
#include "vulkan_buffer.h"
#include <buffer.h>
#include <lmlib/variant_visitor.h>

namespace lmgl
{

void vulkan_buffer_base::update_region(
  vk::DeviceMemory memory,
  const void *data,
  size_t size,
  size_t offset)
{
    auto vpos_map_data_address =
      renderer->vk_device->mapMemory(memory, offset, size);
    memcpy(vpos_map_data_address, data, size);
    renderer->vk_device->unmapMemory(memory);
}

vk::BufferUsageFlags usage_map[] = {vk::BufferUsageFlagBits::eVertexBuffer,
                                    vk::BufferUsageFlagBits::eIndexBuffer,
                                    vk::BufferUsageFlagBits::eUniformBuffer};

void vulkan_buffer_base::init_buffer_usage(const buffer_init &init)
{
    vk_usage =
      usage_map[(int)init.usage] | vk::BufferUsageFlagBits::eTransferDst;
}

vulkan_buffer_base::vulkan_buffer_base(
  const buffer_init &init,
  vulkan_renderer *renderer)
    : renderer{renderer}
{
    init_buffer_usage(init);
}

vk::UniqueDeviceMemory
  vulkan_buffer_base::create_memory(const vk::UniqueBuffer &buffer)
{
    auto mem_requirements =
      renderer->vk_device->getBufferMemoryRequirements(*buffer);

    auto mem_type_index = renderer->find_memory_type_index(
      mem_requirements.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent);

    auto buf_alloc_info = vk::MemoryAllocateInfo{}
                            .setAllocationSize(mem_requirements.size)
                            .setMemoryTypeIndex(mem_type_index);

    return renderer->vk_device->allocateMemoryUnique(buf_alloc_info);
}

vk::UniqueBuffer vulkan_buffer_base::create_buffer(size_t size)
{
    assert(size > 0);

    return renderer->vk_device->createBufferUnique(
      vk::BufferCreateInfo{}.setSize(size).setUsage(vk_usage).setSharingMode(
        vk::SharingMode::eExclusive));
}

void vulkan_buffer_base::update_memory(
  vk::DeviceMemory memory,
  lm::array_proxy<const char> data)
{
    if (data.first_element_addr)
        update_region(memory, data.data(), data.size(), 0);
}
} // namespace lmgl
