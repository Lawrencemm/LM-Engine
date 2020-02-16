#include <lmlib/range.h>
#include <lmlib/variant_visitor.h>

#ifdef __linux__
#include <lmpl/xcb.h>
#elif defined _WIN32
#include <lmpl/windows.h>
#endif

#include <stage.h>

#include "stage_helpers.h"
#include "vulkan_exceptions.h"
#include "vulkan_renderer.h"
#include "vulkan_window_stage.h"

namespace lmgl
{
stage vulkan_renderer::create_stage(const stage_init &init)
{
    return std::make_unique<vulkan_window_stage>(*this, init.output_window);
}

vulkan_window_stage::vulkan_window_stage(
  vulkan_renderer &renderer_in,
  lmpl::iwindow *window)
    : output_window{window}
{
    renderer = &renderer_in;
    dpi_scale = window->dpi_scale();
    create_surface();
    create_images();
    initialised = true;
}

vulkan_window_stage::operator bool() { return initialised; }

void vulkan_window_stage::resize(lmpl::iwindow &window) { return; }

void vulkan_window_stage::create_images()
{
    // Set the colour image format.
    auto formats =
      renderer->vk_physical_device.getSurfaceFormatsKHR(surface.get());
    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
    {
        colour_image_format = vk::Format::eB8G8R8A8Srgb;
    }
    else if (
      std::find_if(
        formats.begin(), formats.end(), [](vk::SurfaceFormatKHR &format) {
            return format.format == vk::Format::eB8G8R8A8Srgb;
        }) != formats.end())
    {
        colour_image_format = vk::Format::eB8G8R8A8Srgb;
    }
    else
    {
        throw NoSuitableFormat(renderer->vk_physical_device);
    }

    // Set the swap chain image size.
    auto surfaceCaps =
      renderer->vk_physical_device.getSurfaceCapabilitiesKHR(*surface);
    if (surfaceCaps.currentExtent.width == 0xFFFFFFFF)
    {
        throw NotImplemented{"SurfaceKHR size determined by the extent"
                             " of a swapchain targeting the surface."};
    }
    else
    {
        currentExtent = surfaceCaps.currentExtent;
    }
    min_image_count = surfaceCaps.minImageCount;

    auto swapchainCreateInfoKHR =
      vk::SwapchainCreateInfoKHR{}
        .setSurface(*surface)
        .setMinImageCount(surfaceCaps.minImageCount)
        .setImageFormat(colour_image_format)
        .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
        .setImageExtent(currentExtent)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setImageArrayLayers(1)
        .setQueueFamilyIndexCount(1)
        .setPQueueFamilyIndices(&renderer->main_queue_family_index)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(vk::PresentModeKHR::eFifo);

    swapchain =
      renderer->vk_device->createSwapchainKHRUnique(swapchainCreateInfoKHR);

    colour_images = renderer->vk_device->getSwapchainImagesKHR(*swapchain);

    image_count = colour_images.size();

    // Craete depth images, views and colour image views.
    for (auto i : lm::range(colour_images.size()))
    {
        auto [depth_image, depth_memory, depth_view] =
          create_depth_stencil_image(
            renderer, vk::Format::eD32SfloatS8Uint, currentExtent);

        depth_images.emplace_back(std::move(depth_image));
        depth_image_memories.emplace_back(std::move(depth_memory));
        depth_image_views.emplace_back(std::move(depth_view));

        auto colour_image_view_info =
          vk::ImageViewCreateInfo{}
            .setImage(colour_images[i])
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(colour_image_format)
            .setSubresourceRange(
              vk::ImageSubresourceRange{}
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setLayerCount(1)
                .setLevelCount(1));

        colour_image_views.emplace_back(
          renderer->vk_device->createImageViewUnique(colour_image_view_info));
    }

    for (auto i : lm::range(colour_images.size()))
    {
        std::array attachments{colour_image_views[i].get(),
                               depth_image_views[i].get()};
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

        frame_buffers.emplace_back(
          renderer->vk_device->createFramebufferUnique(framebufferCreateInfo));
    }
}

void vulkan_window_stage::create_surface()
{
#ifdef __linux__
    auto xcb_window = dynamic_cast<lmpl::ixcb_window *>(output_window);

    if (!xcb_window)
        throw std::runtime_error{
          "Graphics Vulkan stage creation: window does not "
          "derive from internal window interface."};

    auto create_info = vk::XcbSurfaceCreateInfoKHR{}
                         .setConnection(xcb_window->get_connection())
                         .setWindow(xcb_window->get_window());

    surface = renderer->vk_instance->createXcbSurfaceKHRUnique(create_info);

#elif defined _WIN32
    auto windows_window = dynamic_cast<lmpl::iwindows_window *>(output_window);

    if (!windows_window)
        throw std::runtime_error{
          "Graphics Vulkan stage creation: window does not "
          "derive from internal window interface."};

    auto create_info = vk::Win32SurfaceCreateInfoKHR{}
                         .setHinstance(GetModuleHandle(NULL))
                         .setHwnd(windows_window->get_hwnd());
    surface = renderer->vk_instance->createWin32SurfaceKHRUnique(create_info);

#endif

    // Assert that the renderer can present images.
    if (!renderer->vk_physical_device.getSurfaceSupportKHR(
          renderer->main_queue_family_index, *surface))
    {
        throw QueueCannotPresent(renderer->vk_physical_device);
    }
}
} // namespace lmgl
