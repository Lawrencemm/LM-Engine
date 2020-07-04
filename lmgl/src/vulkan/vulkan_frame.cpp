#include "vulkan_frame.h"

#include "vulkan_renderer.h"
#include "vulkan_window_stage.h"

namespace lmgl
{
frame vulkan_window_stage::wait_for_frame()
{
    std::unique_lock lck{image_acquire_mutex};
    image_acquire_cv.wait(lck, [this] {
        return acquired_image_count <= image_count - min_image_count;
    });
    lck.unlock();

    auto backbuffer_acquired_semaphore =
      renderer->vk_device->createSemaphoreUnique(vk::SemaphoreCreateInfo{});

    auto swapchain_image_index =
      renderer->vk_device
        ->acquireNextImageKHR(
          *swapchain, UINT64_MAX, *backbuffer_acquired_semaphore, vk::Fence{})
        .value;
    acquired_image_count++;

    return std::make_unique<vulkan_frame>(
      this, swapchain_image_index, std::move(backbuffer_acquired_semaphore));
}

vulkan_frame::vulkan_frame(
  vulkan_window_stage *stage,
  uint32_t swapchain_image_index,
  vk::UniqueSemaphore backbuffer_acquired_semaphore)
    : stage{stage},
      frame_buffer{stage->frame_buffers[swapchain_image_index].get()},
      swapchain_image_index{swapchain_image_index},
      command_pool{stage->renderer->vk_device->createCommandPoolUnique(
        vk::CommandPoolCreateInfo{}
          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
          .setQueueFamilyIndex(stage->renderer->main_queue_family_index))},
      command_buffer{
        std::move(stage->renderer->vk_device->allocateCommandBuffersUnique(
          vk::CommandBufferAllocateInfo{}
            .setCommandPool(*command_pool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1))[0])},
      completion_semaphore{stage->renderer->vk_device->createSemaphoreUnique(
        vk::SemaphoreCreateInfo{})},
      backbuffer_acquired_semaphore{std::move(backbuffer_acquired_semaphore)},
      complete_fence{stage->renderer->vk_device->createFenceUnique({})},
      viewport{
        vk::Viewport{}
          .setMinDepth(0)
          .setMaxDepth(1.f)
          .setWidth(stage->currentExtent.width)
          .setHeight(stage->currentExtent.height),
      },
      scissor{{0, 0}, stage->currentExtent}
{
    command_buffer->begin(vk::CommandBufferBeginInfo{}.setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
}

iframe &vulkan_frame::add_buffer_update(
  ibuffer *const buffer,
  lm::array_proxy<const char> data,
  size_t byte_offset)
{
    auto vk_buffer = dynamic_cast<vulkan_buffer *>(buffer);
    command_buffer->updateBuffer(
      vk_buffer->buffer.get(), byte_offset, data.size(), data.data());
    return *this;
}

vulkan_frame &vulkan_frame::build()
{
    std::array clear_values{
      vk::ClearValue{clear_colour_},
      vk::ClearValue{vk::ClearDepthStencilValue{1.f, 0}}};

    auto renderPassBeginInfo =
      vk::RenderPassBeginInfo{}
        .setRenderPass(stage->renderer->render_pass.get())
        .setFramebuffer(frame_buffer)
        .setRenderArea(vk::Rect2D{}.setExtent(stage->currentExtent))
        .setClearValueCount((uint32_t)clear_values.size())
        .setPClearValues(clear_values.data());

    command_buffer->beginRenderPass(
      renderPassBeginInfo, vk::SubpassContents::eInline);

    command_buffer->setViewport(0, 1, &viewport);
    command_buffer->setScissor(0, 1, &scissor);

    for (auto &element : elements)
        element->render(*this);

    command_buffer->endRenderPass();

    command_buffer->end();
    return *this;
}

iframe &vulkan_frame::add(lm::array_proxy<ielement *const> elements)
{
    for (auto element : elements)
    {
        auto vk_element = dynamic_cast<vulkan_element *const>(element);
        this->elements.emplace_back(
          vk_element->create_context_node(viewport, scissor));
    }
    return *this;
}

vk::Viewport get_vk_viewport(viewport const &lm_viewport)
{
    return vk::Viewport{
      lm_viewport.position.x,
      lm_viewport.position.y,
      lm_viewport.size.width,
      lm_viewport.size.height,
      lm_viewport.min_depth,
      lm_viewport.max_depth,
    };
}

iframe &vulkan_frame::add(
  lm::array_proxy<ielement *const> elements,
  struct viewport const &viewport)
{
    for (auto element : elements)
    {
        auto vk_element = dynamic_cast<vulkan_element *const>(element);
        this->elements.emplace_back(
          vk_element->create_context_node(get_vk_viewport(viewport), scissor));
    }
    return *this;
}

iframe &vulkan_frame::add(
  lm::array_proxy<ielement *const> elements,
  lm::point2i scissor_origin,
  lm::size2i scissor_extent)
{
    for (auto element : elements)
    {
        auto vk_element = dynamic_cast<vulkan_element *const>(element);
        this->elements.emplace_back(vk_element->create_context_node(
          viewport,
          {{scissor_origin.x, scissor_origin.y},
           {(unsigned)scissor_extent.width, (unsigned)scissor_extent.height}}));
    }
    return *this;
}

iframe &vulkan_frame::add(
  lm::array_proxy<ielement *const> elements,
  struct viewport const &viewport,
  lm::point2i scissor_origin,
  lm::size2i scissor_extent)
{
    for (auto element : elements)
    {
        auto vk_element = dynamic_cast<vulkan_element *const>(element);
        this->elements.emplace_back(vk_element->create_context_node(
          get_vk_viewport(viewport),
          {{scissor_origin.x, scissor_origin.y},
           {(unsigned)scissor_extent.width, (unsigned)scissor_extent.height}}));
    }
    return *this;
}

iframe &vulkan_frame::clear_colour(std::array<float, 4> colour)
{
    clear_colour_ = colour;
    return *this;
}
iframe &vulkan_frame::submit()
{
    stage->renderer->submit_frame(this);
    return *this;
}

lm::size2u vulkan_frame::size()
{
    return {stage->currentExtent.width, stage->currentExtent.height};
}

void vulkan_frame::wait_complete()
{
    stage->renderer->vk_device->waitForFences(
      1, &complete_fence.get(), VK_TRUE, UINT64_MAX);
}

void vulkan_renderer::submit_frame(vulkan_frame *frame)
{
    vk::PipelineStageFlags submitWaitStage =
      vk::PipelineStageFlagBits::eColorAttachmentOutput;

    auto submitInfo =
      vk::SubmitInfo{}
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&*frame->backbuffer_acquired_semaphore)
        .setSignalSemaphoreCount(1)
        .setPSignalSemaphores(&*frame->completion_semaphore)
        .setCommandBufferCount(1)
        .setPCommandBuffers(&*frame->command_buffer)
        .setPWaitDstStageMask(&submitWaitStage);

    main_queue.submit(submitInfo, frame->complete_fence.get());

    auto presentInfoKHR = vk::PresentInfoKHR{}
                            .setWaitSemaphoreCount(1)
                            .setPWaitSemaphores(&*frame->completion_semaphore)
                            .setSwapchainCount(1)
                            .setPSwapchains(&*frame->stage->swapchain)
                            .setPImageIndices(&frame->swapchain_image_index);

    main_queue.presentKHR(presentInfoKHR);
    {
        std::unique_lock lck{frame->stage->image_acquire_mutex};
        frame->stage->acquired_image_count--;
        frame->stage->image_acquire_cv.notify_one();
    }
}
} // namespace lmgl
