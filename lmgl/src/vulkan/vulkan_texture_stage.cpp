#include <lmlib/range.h>

#include <lmgl/frame.h>
#include <stage.h>

#include "stage_helpers.h"
#include "vulkan_renderer.h"
#include "vulkan_texture_stage.h"

namespace lmgl
{
stage vulkan_renderer::create_texture_stage(lmgl::itexture *texture)
{
    return std::make_unique<vulkan_texture_stage>(
      *this, dynamic_cast<vulkan_texture *>(texture));
}

vulkan_texture_stage::vulkan_texture_stage(
  vulkan_renderer &renderer_in,
  vulkan_texture *texture)
{
    renderer = &renderer_in;
    create_images();
    initialised = true;
}

vulkan_texture_stage::operator bool() { return initialised; }

void vulkan_texture_stage::resize(lmpl::iwindow &window) { return; }

void vulkan_texture_stage::create_images()
{
    std::tie(depth_image, depth_image_memory, depth_image_view) =
      create_depth_stencil_image(
        renderer, vk::Format::eD32Sfloat, currentExtent);

    auto colour_image_view_info =
      vk::ImageViewCreateInfo{}
        .setImage(texture_image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(colour_image_format)
        .setSubresourceRange(vk::ImageSubresourceRange{}
                               .setAspectMask(vk::ImageAspectFlagBits::eColor)
                               .setLayerCount(1)
                               .setLevelCount(1));

    texture_view =
      renderer->vk_device->createImageViewUnique(colour_image_view_info);

    vk::ImageViewCreateInfo depth_image_view_info{
      vk::ImageViewCreateFlags{},
      depth_image.get(),
      vk::ImageViewType::e2D,
      depth_stencil_format,
      vk::ComponentMapping{},
      vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}};

    depth_image_view =
      renderer->vk_device->createImageViewUnique(depth_image_view_info);

    std::array attachments{texture_view.get(), depth_image_view.get()};
    // TODO: Check swapchain image formats are same as existing
    // renderpass format. If not, make intermediary buffer.
    auto framebufferCreateInfo =
      vk::FramebufferCreateInfo{}
        .setRenderPass(renderer->render_pass.get())
        .setAttachmentCount((uint32_t)attachments.size())
        .setPAttachments(attachments.data())
        .setWidth(currentExtent.width)
        .setHeight(currentExtent.height)
        .setLayers(1);

    frame_buffer =
      renderer->vk_device->createFramebufferUnique(framebufferCreateInfo);
}

frame lmgl::vulkan_texture_stage::wait_for_frame() { return lmgl::frame(); }
} // namespace lmgl
