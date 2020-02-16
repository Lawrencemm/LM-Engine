#pragma once

#include <vulkan/vulkan.hpp>

#include <stage.h>

#include "vulkan_renderer.h"
#include "vulkan_texture.h"

namespace lmgl
{

class vulkan_scene;

class vulkan_texture_stage : public istage
{
    friend class vulkan_renderer;

  public:
    vulkan_texture_stage(vulkan_renderer &renderer_in, vulkan_texture *texture);

    operator bool();
    frame wait_for_frame() override;

  private:
    void create_images();
    void resize(lmpl::iwindow &window);

  public:
    vk::Extent2D currentExtent;
    uint32_t image_count, acquired_image_count{0};
    std::condition_variable image_acquire_cv;
    std::mutex image_acquire_mutex;

    vk::Format colour_image_format;
    vk::Image texture_image;
    vk::UniqueImageView texture_view;

    vk::Format depth_stencil_format;
    vk::UniqueImage depth_image;
    vk::UniqueDeviceMemory depth_image_memory;
    vk::UniqueImageView depth_image_view;
    vk::UniqueFramebuffer frame_buffer;

    vulkan_renderer *renderer;
    vulkan_scene *scene{nullptr};
    bool initialised{false};
};
} // namespace lmgl