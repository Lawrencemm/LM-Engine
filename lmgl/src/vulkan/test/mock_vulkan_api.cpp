#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
  const VkInstanceCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(
  VkInstance instance, uint32_t *pPhysicalDeviceCount,
  VkPhysicalDevice *pPhysicalDevices)
{
    if (!pPhysicalDevices)
    {
        *pPhysicalDeviceCount = 1;
        return VK_SUCCESS;
    }
    *pPhysicalDevices = nullptr;

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(
  VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures)
{
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(
  VkPhysicalDevice physicalDevice, VkFormat format,
  VkFormatProperties *pFormatProperties)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(
  VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type,
  VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags,
  VkImageFormatProperties *pImageFormatProperties)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(
  VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties)
{
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
  VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount,
  VkQueueFamilyProperties *pQueueFamilyProperties)
{
    if (!pQueueFamilyProperties)
    {
        *pQueueFamilyPropertyCount = 1;
        return;
    }
    *pQueueFamilyProperties = VkQueueFamilyProperties{
      VkQueueFlags(
        vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute |
        vk::QueueFlagBits::eTransfer),
      1,
      1,
      vk::Extent3D{16, 16, 16}};
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(
  VkPhysicalDevice physicalDevice,
  VkPhysicalDeviceMemoryProperties *pMemoryProperties)
{
    *pMemoryProperties = vk::PhysicalDeviceMemoryProperties{};
    pMemoryProperties->memoryTypeCount = 1;
    *pMemoryProperties->memoryTypes =
      vk::MemoryType{vk::MemoryPropertyFlagBits::eDeviceLocal, 0};
    pMemoryProperties->memoryHeapCount = 1;
    *pMemoryProperties->memoryHeaps =
      vk::MemoryHeap{16, vk::MemoryHeapFlagBits::eDeviceLocal};
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char *pName);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice device, const char *pName);

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
  VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
  const char *pLayerName, uint32_t *pPropertyCount,
  VkExtensionProperties *pProperties)
{
    if (pProperties)
    {
        throw std::runtime_error(
          "Mock vkEnumerateInstanceExtensionProperties not "
          "implemented to return an actual list.");
    }
    *pPropertyCount = 0;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
  VkPhysicalDevice physicalDevice, const char *pLayerName,
  uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
  uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    if (pProperties)
    {
        throw std::runtime_error("Mock vkEnumerateInstanceLayerProperties not "
                                 "implemented to return an actual list.");
    }
    *pPropertyCount = 0;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
  VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount,
  VkLayerProperties *pProperties)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(
  VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex,
  VkQueue *pQueue)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(
  VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits,
  VkFence fence)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(
  VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo,
  const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkFreeMemory(
  VkDevice device, VkDeviceMemory memory,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(
  VkDevice device, VkDeviceMemory memory, VkDeviceSize offset,
  VkDeviceSize size, VkMemoryMapFlags flags, void **ppData)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(
  VkDevice device, uint32_t memoryRangeCount,
  const VkMappedMemoryRange *pMemoryRanges)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(
  VkDevice device, uint32_t memoryRangeCount,
  const VkMappedMemoryRange *pMemoryRanges)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(
  VkDevice device, VkDeviceMemory memory, VkDeviceSize *pCommittedMemoryInBytes)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(
  VkDevice device, VkBuffer buffer, VkDeviceMemory memory,
  VkDeviceSize memoryOffset)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(
  VkDevice device, VkImage image, VkDeviceMemory memory,
  VkDeviceSize memoryOffset)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(
  VkDevice device, VkBuffer buffer, VkMemoryRequirements *pMemoryRequirements)
{
    *pMemoryRequirements =
      vk::MemoryRequirements{16, 16, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
}

VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(
  VkDevice device, VkImage image, VkMemoryRequirements *pMemoryRequirements)
{
    *pMemoryRequirements =
      vk::MemoryRequirements{16, 16, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements(
  VkDevice device, VkImage image, uint32_t *pSparseMemoryRequirementCount,
  VkSparseImageMemoryRequirements *pSparseMemoryRequirements)
{
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(
  VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type,
  VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling,
  uint32_t *pPropertyCount, VkSparseImageFormatProperties *pProperties)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(
  VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo *pBindInfo,
  VkFence fence)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(
  VkDevice device, const VkFenceCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkFence *pFence)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFence(
  VkDevice device, VkFence fence, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL
vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(
  VkDevice device, uint32_t fenceCount, const VkFence *pFences,
  VkBool32 waitAll, uint64_t timeout)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(
  VkDevice device, const VkSemaphoreCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(
  VkDevice device, VkSemaphore semaphore,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(
  VkDevice device, const VkEventCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkEvent *pEvent)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(
  VkDevice device, VkEvent event, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(
  VkDevice device, const VkQueryPoolCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkQueryPool *pQueryPool)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(
  VkDevice device, VkQueryPool queryPool,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(
  VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
  uint32_t queryCount, size_t dataSize, void *pData, VkDeviceSize stride,
  VkQueryResultFlags flags)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(
  VkDevice device, const VkBufferCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(
  VkDevice device, VkBuffer buffer, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(
  VkDevice device, const VkBufferViewCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkBufferView *pView)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(
  VkDevice device, VkBufferView bufferView,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(
  VkDevice device, const VkImageCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkImage *pImage)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyImage(
  VkDevice device, VkImage image, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(
  VkDevice device, VkImage image, const VkImageSubresource *pSubresource,
  VkSubresourceLayout *pLayout)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(
  VkDevice device, const VkImageViewCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkImageView *pView)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(
  VkDevice device, VkImageView imageView,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
  VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(
  VkDevice device, VkShaderModule shaderModule,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(
  VkDevice device, const VkPipelineCacheCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkPipelineCache *pPipelineCache)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(
  VkDevice device, VkPipelineCache pipelineCache,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(
  VkDevice device, VkPipelineCache pipelineCache, size_t *pDataSize,
  void *pData)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(
  VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount,
  const VkPipelineCache *pSrcCaches)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
  VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
  const VkGraphicsPipelineCreateInfo *pCreateInfos,
  const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(
  VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
  const VkComputePipelineCreateInfo *pCreateInfos,
  const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(
  VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(
  VkDevice device, const VkPipelineLayoutCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkPipelineLayout *pPipelineLayout)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(
  VkDevice device, VkPipelineLayout pipelineLayout,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(
  VkDevice device, const VkSamplerCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkSampler *pSampler)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySampler(
  VkDevice device, VkSampler sampler, const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(
  VkDevice device, const VkDescriptorSetLayoutCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkDescriptorSetLayout *pSetLayout)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(
  VkDevice device, VkDescriptorSetLayout descriptorSetLayout,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(
  VkDevice device, const VkDescriptorPoolCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkDescriptorPool *pDescriptorPool)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(
  VkDevice device, VkDescriptorPool descriptorPool,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(
  VkDevice device, VkDescriptorPool descriptorPool,
  VkDescriptorPoolResetFlags flags)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(
  VkDevice device, const VkDescriptorSetAllocateInfo *pAllocateInfo,
  VkDescriptorSet *pDescriptorSets)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(
  VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount,
  const VkDescriptorSet *pDescriptorSets)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(
  VkDevice device, uint32_t descriptorWriteCount,
  const VkWriteDescriptorSet *pDescriptorWrites, uint32_t descriptorCopyCount,
  const VkCopyDescriptorSet *pDescriptorCopies)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(
  VkDevice device, const VkFramebufferCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkFramebuffer *pFramebuffer)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(
  VkDevice device, VkFramebuffer framebuffer,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(
  VkDevice device, const VkRenderPassCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(
  VkDevice device, VkRenderPass renderPass,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(
  VkDevice device, VkRenderPass renderPass, VkExtent2D *pGranularity)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(
  VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(
  VkDevice device, VkCommandPool commandPool,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(
  VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(
  VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
  VkCommandBuffer *pCommandBuffers)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(
  VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount,
  const VkCommandBuffer *pCommandBuffers)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(
  VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo *pBeginInfo)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(
  VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(
  VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
  VkPipeline pipeline)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(
  VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount,
  const VkViewport *pViewports)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(
  VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount,
  const VkRect2D *pScissors)
{
}

VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(
  VkCommandBuffer commandBuffer, float depthBiasConstantFactor,
  float depthBiasClamp, float depthBiasSlopeFactor)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(
  VkCommandBuffer commandBuffer, const float blendConstants[4])
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(
  VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(
  VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
  uint32_t compareMask)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(
  VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
  uint32_t writeMask)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(
  VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
  uint32_t reference)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(
  VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
  VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount,
  const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount,
  const uint32_t *pDynamicOffsets)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(
  VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
  VkIndexType indexType)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(
  VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount,
  const VkBuffer *pBuffers, const VkDeviceSize *pOffsets)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdDraw(
  VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
  uint32_t firstVertex, uint32_t firstInstance)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(
  VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount,
  uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(
  VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
  uint32_t drawCount, uint32_t stride)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(
  VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
  uint32_t drawCount, uint32_t stride)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(
  VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
  uint32_t groupCountZ)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(
  VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(
  VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
  uint32_t regionCount, const VkBufferCopy *pRegions)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(
  VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout,
  VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount,
  const VkImageCopy *pRegions)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(
  VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout,
  VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount,
  const VkImageBlit *pRegions, VkFilter filter)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(
  VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage,
  VkImageLayout dstImageLayout, uint32_t regionCount,
  const VkBufferImageCopy *pRegions)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(
  VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout,
  VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(
  VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
  VkDeviceSize dataSize, const void *pData)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(
  VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
  VkDeviceSize size, uint32_t data)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(
  VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
  const VkClearColorValue *pColor, uint32_t rangeCount,
  const VkImageSubresourceRange *pRanges)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(
  VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
  const VkClearDepthStencilValue *pDepthStencil, uint32_t rangeCount,
  const VkImageSubresourceRange *pRanges)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(
  VkCommandBuffer commandBuffer, uint32_t attachmentCount,
  const VkClearAttachment *pAttachments, uint32_t rectCount,
  const VkClearRect *pRects)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(
  VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout,
  VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount,
  const VkImageResolve *pRegions)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(
  VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(
  VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents(
  VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent *pEvents,
  VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
  uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
  uint32_t bufferMemoryBarrierCount,
  const VkBufferMemoryBarrier *pBufferMemoryBarriers,
  uint32_t imageMemoryBarrierCount,
  const VkImageMemoryBarrier *pImageMemoryBarriers)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(
  VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask,
  VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
  uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
  uint32_t bufferMemoryBarrierCount,
  const VkBufferMemoryBarrier *pBufferMemoryBarriers,
  uint32_t imageMemoryBarrierCount,
  const VkImageMemoryBarrier *pImageMemoryBarriers)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(
  VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query,
  VkQueryControlFlags flags)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(
  VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(
  VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery,
  uint32_t queryCount)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(
  VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage,
  VkQueryPool queryPool, uint32_t query)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(
  VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery,
  uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset,
  VkDeviceSize stride, VkQueryResultFlags flags)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(
  VkCommandBuffer commandBuffer, VkPipelineLayout layout,
  VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
  const void *pValues)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(
  VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo *pRenderPassBegin,
  VkSubpassContents contents)
{
}

VKAPI_ATTR void VKAPI_CALL
vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
}

VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {}

VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(
  VkCommandBuffer commandBuffer, uint32_t commandBufferCount,
  const VkCommandBuffer *pCommandBuffers)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(
  VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(
  VkDevice device, VkSwapchainKHR swapchain,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(
  VkDevice device, VkSwapchainKHR swapchain, uint32_t *pSwapchainImageCount,
  VkImage *pSwapchainImages)
{
    if (!pSwapchainImages)
    {
        *pSwapchainImageCount = 1;
        return VK_SUCCESS;
    }
    *pSwapchainImages = nullptr;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(
  VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout,
  VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupPresentCapabilitiesKHR(
  VkDevice device,
  VkDeviceGroupPresentCapabilitiesKHR *pDeviceGroupPresentCapabilities)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupSurfacePresentModesKHR(
  VkDevice device, VkSurfaceKHR surface,
  VkDeviceGroupPresentModeFlagsKHR *pModes)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDevicePresentRectanglesKHR(
  VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pRectCount,
  VkRect2D *pRects)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImage2KHR(
  VkDevice device, const VkAcquireNextImageInfoKHR *pAcquireInfo,
  uint32_t *pImageIndex)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(
  VkInstance instance, VkSurfaceKHR surface,
  const VkAllocationCallbacks *pAllocator)
{
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(
  VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex,
  VkSurfaceKHR surface, VkBool32 *pSupported)
{
    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
  VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
  VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(
  VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
  uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats)
{
    if (!pSurfaceFormats)
    {
        *pSurfaceFormatCount = 1;
        return VK_SUCCESS;
    }
    *pSurfaceFormats = VkSurfaceFormatKHR{VK_FORMAT_UNDEFINED,
                                          VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(
  VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
  uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes)
{
    return VK_SUCCESS;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    return []() { return; };
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
  VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback)
{
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
  VkInstance instance, VkDebugReportCallbackEXT callback,
  const VkAllocationCallbacks *pAllocator)
{
}
