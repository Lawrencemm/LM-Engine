#include "stage_helpers.h"

#include "vulkan_renderer.h"

namespace lmgl
{
auto create_depth_stencil_image(
  vulkan_renderer *renderer,
  vk::Format format,
  vk::Extent2D const &extent)
  -> std::tuple<vk::UniqueImage, vk::UniqueDeviceMemory, vk::UniqueImageView>
{
    vk::ImageCreateInfo depth_image_create_info{
      vk::ImageCreateFlags{},
      vk::ImageType::e2D,
      format,
      vk::Extent3D{extent.width, extent.height, 1},
      1,
      1,
      vk::SampleCountFlagBits::e1,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eDepthStencilAttachment,
      vk::SharingMode::eExclusive,
      1,
      &renderer->main_queue_family_index,
      vk::ImageLayout::eUndefined};

    auto image =
      renderer->vk_device->createImageUnique(depth_image_create_info);

    auto memory_requirements =
      renderer->vk_device->getImageMemoryRequirements(image.get());

    vk::MemoryAllocateInfo alloc_info{
      memory_requirements.size,
      renderer->find_memory_type_index(
        memory_requirements.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eDeviceLocal)};

    auto mem = renderer->vk_device->allocateMemoryUnique(alloc_info);

    renderer->vk_device->bindImageMemory(image.get(), mem.get(), 0);

    vk::ImageViewCreateInfo depth_image_view_info{
      vk::ImageViewCreateFlags{},
      image.get(),
      vk::ImageViewType::e2D,
      format,
      vk::ComponentMapping{},
      vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth |
                                  vk::ImageAspectFlagBits::eStencil,
                                0,
                                1,
                                0,
                                1}};

    auto depth_view =
      renderer->vk_device->createImageViewUnique(depth_image_view_info);

    return {std::move(image), std::move(mem), std::move(depth_view)};
}
} // namespace lmgl
