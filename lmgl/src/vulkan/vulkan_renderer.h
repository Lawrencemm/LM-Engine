#pragma once

#include <queue>

#include <vulkan/vulkan.hpp>

#include <lmlib/remote_executor.h>

#include "material.h"

#include <renderer.h>

namespace lmgl
{
class vulkan_frame;

class vulkan_renderer : public irenderer
{
  public:
    vulkan_renderer(const renderer_init &init);

    stage create_stage(const stage_init &init) override;
    geometry create_geometry(const geometry_init &init) override;
    buffer create_buffer(const buffer_init &init) override;
    texture create_texture(const texture_init &init) override;
    material create_material(material_init const &init) override;
    irenderer &destroy_material(material) override;

    // Utility functions for Vulkan initialisation.
    void initInstance();
    void initDevice();
    static bool GetGraphicsQueue(
      const std::vector<vk::QueueFamilyProperties> &queueFamilyProps,
      unsigned int &mainQueueFamIndex,
      vk::DeviceQueueCreateInfo &queueCreateInfo);
    uint32_t find_memory_type_index(
      uint32_t type_bits,
      vk::MemoryPropertyFlags properties);
    std::vector<const char *> getInstanceExtensions() const;
    std::vector<const char *> getInstanceLayers() const;
    void
      addDebugReportCallback(vk::InstanceCreateInfo &instanceCreateInfo) const;
    void createDebugReportCallback();
    void create_render_pass();

    // Vulkan utility functions.
    vk::UniqueDeviceMemory create_memory(const vk::UniqueBuffer &buffer);
    void submit_frame(vulkan_frame *frame);

    // Vulkan API objects.
    vk::UniqueInstance vk_instance;
    vk::UniqueDebugReportCallbackEXT debug_report_callback;
    vk::PhysicalDevice vk_physical_device;
    vk::UniqueDevice vk_device;
    unsigned main_queue_family_index;
    vk::Queue main_queue;
    vk::UniqueCommandPool commandPool;
    vk::UniqueRenderPass render_pass;

    vk::PipelineDynamicStateCreateInfo object_dynamic_state_info{};
    std::vector<vk::DynamicState> object_dynamic_states;

    lm::remote_executor fence_wait_thread;
    void populate_whole_texture(
      vk::Image image,
      vk::Extent3D extent,
      lm::array_proxy<char const> data);
    stage create_texture_stage(itexture *itexture) override;
};
} // namespace lmgl