#pragma once

#include <vulkan/vulkan.hpp>

namespace lmgl
{
struct vulkan_material
{
    vk::UniqueDescriptorSetLayout descriptor_set_layout;
    vk::UniquePipelineLayout pipeline_layout;
    vk::UniquePipeline pipeline;
    vk::IndexType index_type;
};
} // namespace lmgl
