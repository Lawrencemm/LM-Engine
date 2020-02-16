#pragma once

#include <texture.h>

#include "vulkan_renderer.h"

namespace lmgl
{
class vulkan_texture : public itexture
{
  public:
    vulkan_texture(vulkan_renderer *renderer, const texture_init &init);

    vulkan_renderer *renderer;

    vk::UniqueImage image;
    vk::UniqueImageView view;
    vk::UniqueSampler sampler;
    vk::UniqueDeviceMemory memory;
    vk::UniqueBuffer staging_buffer;
    vk::UniqueDeviceMemory staging_memory;
};
} // namespace lmgl
