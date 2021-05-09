#include "vulkan_exceptions.h"

using namespace std::literals::string_literals;

namespace lmgl
{

NoGraphicsQueueException::NoGraphicsQueueException(
  const vk::PhysicalDevice &physicalDevice)
    : vulkan_exception{
        "Device "s + physicalDevice.getProperties().deviceName.data() +
        " has no queue family with graphics capabilities."}
{
}

QueueCannotPresent::QueueCannotPresent(const vk::PhysicalDevice &physicalDevice)
    : vulkan_exception{
        "Device "s + physicalDevice.getProperties().deviceName.data() +
        " main queue cannot present to a win32 KHR surface."}
{
}

NoSuitableFormat::NoSuitableFormat(const vk::PhysicalDevice &physicalDevice)
    : vulkan_exception{
        "Device "s + physicalDevice.getProperties().deviceName.data() +
        " doesn't support B8G8R8A8UNORM surface format."}
{
}

NotImplemented::NotImplemented(const std::string functionalityDesc)
    : vulkan_exception{"Not implemented: " + functionalityDesc}
{
}
} // namespace lmgl