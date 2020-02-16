#pragma once

#include <stdexcept>
#include <vulkan/vulkan.hpp>

namespace lmgl
{

class vulkan_exception : public std::runtime_error
{
    using runtime_error::runtime_error;
};

class NoGraphicsQueueException : public vulkan_exception
{
  public:
    explicit NoGraphicsQueueException(const vk::PhysicalDevice &physicalDevice);
    using vulkan_exception::vulkan_exception;
};

class QueueCannotPresent : public vulkan_exception
{
  public:
    explicit QueueCannotPresent(const vk::PhysicalDevice &physicalDevice);
    using vulkan_exception::vulkan_exception;
};

class NoSuitableFormat : public vulkan_exception
{
  public:
    explicit NoSuitableFormat(const vk::PhysicalDevice &physicalDevice);
    using vulkan_exception::vulkan_exception;
};

class NotImplemented : public vulkan_exception
{
  public:
    explicit NotImplemented(const std::string functionality_desc);
    using vulkan_exception::vulkan_exception;
};
} // namespace lmgl