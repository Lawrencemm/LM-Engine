#pragma once

#include <tuple>
#include <vulkan/vulkan.hpp>

namespace lmgl
{
class vulkan_renderer;

std::tuple<vk::UniqueImage, vk::UniqueDeviceMemory, vk::UniqueImageView>
  create_depth_stencil_image(
    vulkan_renderer *renderer,
    vk::Format format,
    vk::Extent2D const &extent);
} // namespace lmgl
