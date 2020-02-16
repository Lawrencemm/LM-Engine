#pragma once

#include <vulkan/vulkan.hpp>

#include "../vulkan_renderer.h"

namespace lmgl
{
class vulkan_buffer_base : public virtual ibuffer
{
  public:
    vulkan_buffer_base(const buffer_init &init, vulkan_renderer *renderer);
    void
      update_memory(vk::DeviceMemory memory, lm::array_proxy<const char> data);

    vulkan_renderer *renderer;
    vk::BufferUsageFlags vk_usage;

    void init_buffer_usage(const buffer_init &init);
    vk::UniqueBuffer create_buffer(size_t size);
    vk::UniqueDeviceMemory create_memory(const vk::UniqueBuffer &buffer);
    void update_region(
      vk::DeviceMemory memory, const void *data, size_t size, size_t offset);
};
} // namespace lmgl
