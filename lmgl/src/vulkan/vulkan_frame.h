#pragma once

#include <frame.h>

#include "buffer/vulkan_buffer.h"
#include "vulkan_element.h"
#include "vulkan_window_stage.h"

namespace lmgl
{
class vulkan_scene;
class vulkan_frame_element;

class vulkan_frame : public iframe
{
  public:
    iframe &submit() override;

  public:
    void wait_complete() override;
    lm::size2u size() override;
    explicit vulkan_frame(
      vulkan_window_stage *stage,
      uint32_t swapchain_image_index,
      vk::UniqueSemaphore backbuffer_acquired_semaphore);

    // render_frame_interface implementation.
    iframe &add_buffer_update(
      ibuffer *constbuffer,
      lm::array_proxy<const char> data,
      size_t byte_offset) override;

    iframe &add(lm::array_proxy<ielement *const> elements) override;
    iframe &add(
      lm::array_proxy<ielement *const> elements,
      struct viewport const &viewport,
      lm::point2i scissor_origin,
      lm::size2i scissor_extent) override;
    iframe &add(
      lm::array_proxy<ielement *const> elements,
      const viewport &viewport) override;
    iframe &add(
      lm::array_proxy<ielement *const> elements,
      lm::point2i scissor_origin,
      lm::size2i scissor_extent) override;

    vulkan_frame &build() override;
    iframe &clear_colour(std::array<float, 4> colour) override;

    vulkan_window_stage *stage;
    vk::Framebuffer frame_buffer;
    uint32_t swapchain_image_index;

    std::vector<lm::reference<vulkan_frame_element>> elements;

    vk::UniqueCommandPool command_pool;
    vk::UniqueCommandBuffer command_buffer;
    vk::UniqueSemaphore completion_semaphore, backbuffer_acquired_semaphore;
    vk::UniqueFence complete_fence;
    vk::Viewport viewport;
    vk::Rect2D scissor;

    std::array<float, 4> clear_colour_{0.f, 0.f, 0.f, 1.f};
};
} // namespace lmgl
