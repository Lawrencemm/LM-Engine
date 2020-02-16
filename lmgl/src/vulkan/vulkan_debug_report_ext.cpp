#include <vulkan/vulkan.h>

extern PFN_vkCreateDebugReportCallbackEXT createDebugReportCallbackFn;
extern PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallbackFn;

// The stub functions not defined by the Vulkan SDK.
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
  VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
  const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback)
{
    return createDebugReportCallbackFn(
      instance, pCreateInfo, pAllocator, pCallback);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
  VkInstance instance, VkDebugReportCallbackEXT callback,
  const VkAllocationCallbacks *pAllocator)
{
    return destroyDebugReportCallbackFn(instance, callback, pAllocator);
}
