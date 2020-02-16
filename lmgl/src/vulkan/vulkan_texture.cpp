#include "vulkan_texture.h"

namespace lmgl
{
texture vulkan_renderer::create_texture(const texture_init &init)
{
    return std::make_unique<vulkan_texture>(this, init);
}

vulkan_texture::vulkan_texture(
  vulkan_renderer *renderer,
  const texture_init &init)
    : renderer{renderer}
{
    vk::Extent3D texture_extent{
      init.dimensions.width, init.dimensions.height, 1};

    image = renderer->vk_device->createImageUnique(vk::ImageCreateInfo{
      vk::ImageCreateFlags{},
      vk::ImageType::e2D,
      vk::Format::eR8Unorm,
      texture_extent,
      1,
      1,
      vk::SampleCountFlagBits::e1,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
      vk::SharingMode::eExclusive,
      1,
      &renderer->main_queue_family_index,
      vk::ImageLayout::eUndefined,
    });

    auto reqs = renderer->vk_device->getImageMemoryRequirements(image.get());

    memory = renderer->vk_device->allocateMemoryUnique(vk::MemoryAllocateInfo{
      reqs.size,
      renderer->find_memory_type_index(
        reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)});

    renderer->vk_device->bindImageMemory(image.get(), memory.get(), 0);

    view = renderer->vk_device->createImageViewUnique(vk::ImageViewCreateInfo{
      vk::ImageViewCreateFlags{},
      image.get(),
      vk::ImageViewType::e2D,
      vk::Format::eR8Unorm,
      vk::ComponentMapping{},
      vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
    });

    renderer->populate_whole_texture(image.get(), texture_extent, init.data);

    sampler = renderer->vk_device->createSamplerUnique(vk::SamplerCreateInfo{});
}
} // namespace lmgl
