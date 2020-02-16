#pragma once

#include <vulkan/vulkan.hpp>

#include <stage.h>

#include "vulkan_renderer.h"

namespace lmgl
{

class vulkan_scene;

class vulkan_window_stage : public istage
{
    friend class vulkan_renderer;

  public:
    vulkan_window_stage(vulkan_renderer &renderer_in, lmpl::iwindow *window);

    operator bool();
    frame wait_for_frame() override;

  private:
    void create_surface();
    void create_images();
    void resize(lmpl::iwindow &window);

  public:
    vk::Extent2D currentExtent;
    vk::UniqueSurfaceKHR surface;
    uint32_t min_image_count, image_count, acquired_image_count{0};
    vk::UniqueSwapchainKHR swapchain;
    std::condition_variable image_acquire_cv;
    std::mutex image_acquire_mutex;

    vk::Format colour_image_format;
    std::vector<vk::Image> colour_images;
    std::vector<vk::UniqueImageView> colour_image_views;

    vk::Format depth_stencil_format;
    std::vector<vk::UniqueImage> depth_images;
    std::vector<vk::UniqueDeviceMemory> depth_image_memories;
    std::vector<vk::UniqueImageView> depth_image_views;
    std::vector<vk::UniqueFramebuffer> frame_buffers;

    vulkan_renderer *renderer;
    vulkan_scene *scene{nullptr};
    bool initialised{false};

    lmpl::iwindow *output_window;
    float dpi_scale;
};
} // namespace lmgl