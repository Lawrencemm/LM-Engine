#include <lmlib/application.h>
#include <lmlib/enumerate.h>
#include <lmlib/range.h>
#include <fmt/format.h>

#include "vulkan_exceptions.h"
#include "vulkan_renderer.h"

PFN_vkCreateDebugReportCallbackEXT createDebugReportCallbackFn;
PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallbackFn;

namespace lmgl
{

vulkan_renderer::vulkan_renderer(const renderer_init &init)
{
    initInstance();
    initDevice();
    commandPool = vk_device->createCommandPoolUnique(vk::CommandPoolCreateInfo{
      vk::CommandPoolCreateFlags{}, main_queue_family_index});
    create_render_pass();

    object_dynamic_states.emplace_back(vk::DynamicState::eViewport);
    object_dynamic_states.emplace_back(vk::DynamicState::eLineWidth);
    object_dynamic_states.emplace_back(vk::DynamicState::eScissor);

    object_dynamic_state_info.setPDynamicStates(object_dynamic_states.data())
      .setDynamicStateCount((uint32_t)object_dynamic_states.size());
}

void vulkan_renderer::initInstance()
{
    auto appInfo = vk::ApplicationInfo{}
                     .setPEngineName("lmengine")
                     .setEngineVersion(VK_MAKE_VERSION(0, 1, 0))
                     .setPApplicationName("editor_app")
                     .setApplicationVersion(VK_MAKE_VERSION(0, 1, 0))
                     .setApiVersion(VK_MAKE_VERSION(1, 1, 0));
    auto enabledExtensions = getInstanceExtensions();
    auto enabledLayers = getInstanceLayers();
    auto instanceCreateInfo =
      vk::InstanceCreateInfo{}
        .setPApplicationInfo(&appInfo)
        .setEnabledLayerCount((uint32_t)enabledLayers.size())
        .setPpEnabledLayerNames(enabledLayers.data())
        .setEnabledExtensionCount((uint32_t)enabledExtensions.size())
        .setPpEnabledExtensionNames(enabledExtensions.data());
#if !defined(NDEBUG)
    // addDebugReportCallback(instanceCreateInfo);
#endif
    vk_instance = vk::createInstanceUnique(instanceCreateInfo);
#if !defined(NDEBUG)
    createDebugReportCallback();
#endif
}

std::vector<const char *> vulkan_renderer::getInstanceLayers() const
{
    auto layers = vk::enumerateInstanceLayerProperties();
    std::vector<const char *> enabledLayers;
#if !defined(NDEBUG)
    enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
    // enabledLayers.push_back("VK_LAYER_LUNARG_api_dump");
#endif
    return enabledLayers;
}

std::vector<const char *> vulkan_renderer::getInstanceExtensions() const
{
    auto extensions = vk::enumerateInstanceExtensionProperties();
    std::vector<const char *> enabledExtensions = {
      VK_KHR_SURFACE_EXTENSION_NAME};
#ifdef __linux__
    enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

#ifdef _WIN32
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
//  enabledExtensions.push_back(
//      VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

#ifndef NDEBUG
    enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
    return enabledExtensions;
}

void vulkan_renderer::initDevice()
{
    vk_physical_device = vk_instance->enumeratePhysicalDevices()[0];
    auto queueFamilyProps = vk_physical_device.getQueueFamilyProperties();
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    auto found_graphics_queue_fam = false;
    queueCreateInfos.emplace_back();
    found_graphics_queue_fam = GetGraphicsQueue(
      queueFamilyProps, main_queue_family_index, queueCreateInfos.back());
    if (!found_graphics_queue_fam)
        throw NoGraphicsQueueException(vk_physical_device);
    std::array deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    auto supported_features = vk_physical_device.getFeatures();
    if (!supported_features.wideLines)
        throw std::runtime_error{"Vulkan device does not support wide lines."};

    if (!supported_features.fillModeNonSolid)
        throw std::runtime_error(
          "Vulkan device does not support non-fill polygon modes.");

    auto features =
      vk::PhysicalDeviceFeatures{}.setWideLines(1).setFillModeNonSolid(VK_TRUE);
    auto deviceCreateInfo =
      vk::DeviceCreateInfo{}
        .setQueueCreateInfoCount((uint32_t)queueCreateInfos.size())
        .setPQueueCreateInfos(queueCreateInfos.data())
        .setEnabledExtensionCount((uint32_t)deviceExtensions.size())
        .setPpEnabledExtensionNames(deviceExtensions.data())
        .setPEnabledFeatures(&features);
    vk_device = vk_physical_device.createDeviceUnique(deviceCreateInfo);
    main_queue = vk_device->getQueue(main_queue_family_index, 0);
}

bool vulkan_renderer::GetGraphicsQueue(
  const std::vector<vk::QueueFamilyProperties> &queueFamilyProps,
  unsigned int &mainQueueFamIndex,
  vk::DeviceQueueCreateInfo &queueCreateInfo)
{
    float priorities[] = {0.f};
    bool found_graphics_queue_fam{false};

    for (const auto &[queue_family_index, queue_family_props] :
         lm::enumerate(queueFamilyProps))
    {
        if (
          queue_family_props.queueFlags & vk::QueueFlagBits::eGraphics &&
          queue_family_props.queueFlags & vk::QueueFlagBits::eCompute)
        {
            found_graphics_queue_fam = true;
            queueCreateInfo =
              vk::DeviceQueueCreateInfo{}
                .setQueueFamilyIndex((uint32_t)queue_family_index)
                .setQueueCount(1)
                .setPQueuePriorities(priorities);
            mainQueueFamIndex = (uint32_t)queue_family_index;
            break;
        }
    }
    return found_graphics_queue_fam;
}

VkBool32 receiveVkDebugMsg(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objectType,
  uint64_t object,
  size_t location,
  int32_t messageCode,
  const char *pLayerPrefix,
  const char *pMessage,
  void *pUserData)
{
    if (
      flags & (uint32_t)vk::DebugReportFlagBitsEXT::eError ||
      flags & (uint32_t)vk::DebugReportFlagBitsEXT::eWarning)
    {
        auto formatted_msg =
          fmt::format("Vulkan {}: {}\n\n", pLayerPrefix, pMessage);
        std::cout << formatted_msg;
        throw std::runtime_error(formatted_msg);
    }
    return VK_FALSE;
}

void vulkan_renderer::createDebugReportCallback()
{
    createDebugReportCallbackFn =
      (PFN_vkCreateDebugReportCallbackEXT)vk_instance->getProcAddr(
        "vkCreateDebugReportCallbackEXT");
    destroyDebugReportCallbackFn =
      (PFN_vkDestroyDebugReportCallbackEXT)vk_instance->getProcAddr(
        "vkDestroyDebugReportCallbackEXT");
    auto flags = vk::DebugReportFlagBitsEXT::eError |
                 vk::DebugReportFlagBitsEXT::eWarning |
                 vk::DebugReportFlagBitsEXT::eInformation |
                 vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                 vk::DebugReportFlagBitsEXT::eDebug;
    auto dbgCreateInfo =
      vk::DebugReportCallbackCreateInfoEXT{}.setFlags(flags).setPfnCallback(
        (PFN_vkDebugReportCallbackEXT)&receiveVkDebugMsg);
    debug_report_callback =
      vk_instance->createDebugReportCallbackEXTUnique(dbgCreateInfo);
}

void vulkan_renderer::addDebugReportCallback(
  vk::InstanceCreateInfo &instanceCreateInfo) const
{
    auto flags = vk::DebugReportFlagBitsEXT::eError |
                 vk::DebugReportFlagBitsEXT::eWarning |
                 vk::DebugReportFlagBitsEXT::eInformation |
                 vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                 vk::DebugReportFlagBitsEXT::eDebug;
    auto dbgCreateInfo =
      vk::DebugReportCallbackCreateInfoEXT{}.setFlags(flags).setPfnCallback(
        (PFN_vkDebugReportCallbackEXT)&receiveVkDebugMsg);
    instanceCreateInfo.setPNext(&dbgCreateInfo);
}

uint32_t vulkan_renderer::find_memory_type_index(
  uint32_t type_bits,
  vk::MemoryPropertyFlags properties)
{
    auto device_properties = vk_physical_device.getMemoryProperties();
    for (auto i : lm::range(device_properties.memoryTypeCount))
    {
        if (
          type_bits & (1 << i) &&
          (device_properties.memoryTypes[i].propertyFlags & properties) ==
            properties)
        {
            return i;
        }
    }
    throw std::runtime_error("No suitable memory type found.");
}

vk::UniqueDeviceMemory
  vulkan_renderer::create_memory(const vk::UniqueBuffer &buffer)
{
    auto reqs = vk_device->getBufferMemoryRequirements(buffer.get());
    return vk_device->allocateMemoryUnique(
      vk::MemoryAllocateInfo{reqs.size,
                             find_memory_type_index(
                               reqs.memoryTypeBits,
                               vk::MemoryPropertyFlagBits::eHostCoherent |
                                 vk::MemoryPropertyFlagBits::eHostVisible)});
}

void vulkan_renderer::populate_whole_texture(
  vk::Image image,
  vk::Extent3D extent,
  lm::array_proxy<char const> data)
{
    auto staging_buffer = vk_device->createBufferUnique(vk::BufferCreateInfo{
      vk::BufferCreateFlags{},
      data.size(),
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::SharingMode::eExclusive,
      1,
      &main_queue_family_index,
    });

    auto staging_memory = create_memory(staging_buffer);

    vk_device->bindBufferMemory(staging_buffer.get(), staging_memory.get(), 0);

    void *mapped = vk_device->mapMemory(staging_memory.get(), 0, data.size());

    memcpy(mapped, data.data(), data.size());

    vk_device->unmapMemory(staging_memory.get());

    auto cmd_buffers =
      vk_device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{
        commandPool.get(), vk::CommandBufferLevel::ePrimary, 1});

    auto cmd_buffer = cmd_buffers[0].get();

    cmd_buffer.begin(vk::CommandBufferBeginInfo{
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    auto image_barrier = vk::ImageMemoryBarrier{
      {},
      {},
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eTransferDstOptimal,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      image,
      vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
    };

    cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTopOfPipe,
      {},
      0,
      nullptr,
      0,
      nullptr,
      1,
      &image_barrier);

    auto copy = vk::BufferImageCopy{
      0,
      0,
      0,
      vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1},
      {},
      extent,
    };

    cmd_buffer.copyBufferToImage(
      staging_buffer.get(),
      image,
      vk::ImageLayout::eTransferDstOptimal,
      1,
      &copy);

    image_barrier = vk::ImageMemoryBarrier{
      {},
      {},
      vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      image,
      vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
    };

    cmd_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTopOfPipe,
      {},
      0,
      nullptr,
      0,
      nullptr,
      1,
      &image_barrier);

    cmd_buffer.end();

    auto complete_fence = vk_device->createFenceUnique({});

    auto submitInfo =
      vk::SubmitInfo{}.setCommandBufferCount(1).setPCommandBuffers(&cmd_buffer);

    main_queue.submit(submitInfo, complete_fence.get());

    fence_wait_thread.run([this,
                           complete_fence = std::move(complete_fence),
                           b = std::move(staging_buffer),
                           m = std::move(staging_memory),
                           cbs = std::move(cmd_buffers)] {
        vk_device->waitForFences({complete_fence.get()}, VK_TRUE, UINT64_MAX);
    });
}

void vulkan_renderer::create_render_pass()
{
    std::array render_pass_attachments{
      vk::AttachmentDescription{}
        .setFormat(vk::Format::eB8G8R8A8Srgb)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR),
      vk::AttachmentDescription{}
        .setFormat(vk::Format::eD32SfloatS8Uint)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setStencilLoadOp(vk::AttachmentLoadOp::eClear)};

    auto colourAttachmentReference =
      vk::AttachmentReference{}.setAttachment(0).setLayout(
        vk::ImageLayout::eColorAttachmentOptimal);

    auto depth_attachment_reference =
      vk::AttachmentReference{}.setAttachment(1).setLayout(
        vk::ImageLayout::eDepthStencilAttachmentOptimal);

    //    auto stencil_attachment_reference =
    //      vk::AttachmentReference{}.setAttachment(1).setLayout();

    auto subpassDescription =
      vk::SubpassDescription{}
        .setColorAttachmentCount(1)
        .setPColorAttachments(&colourAttachmentReference)
        .setPDepthStencilAttachment(&depth_attachment_reference);

    auto renderPassCreateInfo =
      vk::RenderPassCreateInfo{}
        .setAttachmentCount(render_pass_attachments.size())
        .setPAttachments(render_pass_attachments.data())
        .setSubpassCount(1)
        .setPSubpasses(&subpassDescription);

    render_pass = vk_device->createRenderPassUnique(renderPassCreateInfo);
}

irenderer &vulkan_renderer::destroy_material(material material)
{
    delete reinterpret_cast<vulkan_material *>(material);
    return *this;
}
} // namespace lmgl
