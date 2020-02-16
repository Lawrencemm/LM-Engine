#pragma once

#include "vulkan_buffer_base.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

namespace lmgl
{
class vulkan_renderer;
class vulkan_buffer : public vulkan_buffer_base
{
  public:
    vulkan_buffer(const buffer_init &init, vulkan_renderer *renderer);
    void *map(size_t offset, size_t size);
    void unmap();

    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory memory;
};
} // namespace lmgl