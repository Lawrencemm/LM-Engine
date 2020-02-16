#include "material.h"
#include "vulkan_renderer.h"
#include <lmgl/material.h>

#include <lmlib/enumerate.h>

vk::IndexType itype_to_vk_itype[] = {
  vk::IndexType::eUint16,
  vk::IndexType::eUint32,
};

vk::Format lm_format_to_vk_format[] = {vk::Format::eR32G32Sfloat,
                                       vk::Format::eR32G32B32Sfloat,
                                       vk::Format::eR32G32B32A32Sfloat};

vk::StencilOp lm_stencil_op_to_vk[] = {vk::StencilOp::eIncrementAndClamp};

vk::PolygonMode lm_polygon_mode_to_vk[] = {vk::PolygonMode::eFill,
                                           vk::PolygonMode::eLine};

vk::PolygonMode get_vk_polygon_mode(lmgl::polygon_mode mode)
{
    return lm_polygon_mode_to_vk[(int)mode];
}

unsigned input_type_to_stride[] = {8, 12, 16};

vk::PrimitiveTopology gtype_to_vk_ptype[] = {
  vk::PrimitiveTopology::ePointList,
  vk::PrimitiveTopology::eLineList,
  vk::PrimitiveTopology::eTriangleList};

namespace lmgl
{
material vulkan_renderer::create_material(material_init const &init)
{
    std::vector<vk::DescriptorSetLayoutBinding> uniform_bindings;

    if (init.uniform_size)
    {
        uniform_bindings.push_back(
          vk::DescriptorSetLayoutBinding{}
            .setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eAll));
    }

    if (init.texture)
    {
        uniform_bindings.emplace_back(vk::DescriptorSetLayoutBinding{
          1,
          vk::DescriptorType::eCombinedImageSampler,
          1,
          vk::ShaderStageFlagBits::eFragment,
        });
    }

    auto descriptor_set_layout = vk_device->createDescriptorSetLayoutUnique(
      vk::DescriptorSetLayoutCreateInfo{}
        .setBindingCount(uniform_bindings.size())
        .setPBindings(uniform_bindings.data()));

    auto pipeline_layout = vk_device->createPipelineLayoutUnique(
      vk::PipelineLayoutCreateInfo{}
        .setPSetLayouts(&descriptor_set_layout.get())
        .setSetLayoutCount(1));

    auto v_shader_create_info =
      vk::ShaderModuleCreateInfo{}
        .setPCode(reinterpret_cast<const uint32_t *>(init.vshader_spirv.data()))
        .setCodeSize(init.vshader_spirv.size());

    auto v_shader_module =
      vk_device->createShaderModuleUnique(v_shader_create_info);

    auto stage_create_infos_ = std::vector{
      vk::PipelineShaderStageCreateInfo{}
        .setModule(v_shader_module.get())
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eVertex),
    };
    vk::UniqueShaderModule p_shader_module;
    if (init.pshader_spirv.data())
    {
        auto p_shader_create_info =
          vk::ShaderModuleCreateInfo{}
            .setPCode(
              reinterpret_cast<const uint32_t *>(init.pshader_spirv.data()))
            .setCodeSize(init.pshader_spirv.size());

        p_shader_module =
          vk_device->createShaderModuleUnique(p_shader_create_info);

        stage_create_infos_.emplace_back(
          vk::PipelineShaderStageCreateInfo{}
            .setModule(p_shader_module.get())
            .setPName("main")
            .setStage(vk::ShaderStageFlagBits::eFragment));
    }

    std::vector<vk::VertexInputBindingDescription> bindings;
    std::vector<vk::VertexInputAttributeDescription> attributes;

    for (auto const &[i, input_type] : lm::enumerate(init.vertex_input_types))
    {
        bindings.emplace_back(
          vk::VertexInputBindingDescription{}
            .setBinding(i)
            .setStride(input_type_to_stride[(int)input_type])
            .setInputRate(vk::VertexInputRate::eVertex));
        attributes.emplace_back(
          vk::VertexInputAttributeDescription{}
            .setBinding(i)
            .setFormat(lm_format_to_vk_format[(int)input_type])
            .setLocation(i)
            .setOffset(0));
    }
    auto vertex_input_state_create_info_ =
      vk::PipelineVertexInputStateCreateInfo{}
        .setPVertexBindingDescriptions(bindings.data())
        .setVertexBindingDescriptionCount((uint32_t)bindings.size())
        .setPVertexAttributeDescriptions(attributes.data())
        .setVertexAttributeDescriptionCount((uint32_t)attributes.size());

    auto input_assembly_state =
      vk::PipelineInputAssemblyStateCreateInfo{}.setTopology(
        gtype_to_vk_ptype[(int)init.topology]);

    auto rasterisation_state =
      vk::PipelineRasterizationStateCreateInfo{}.setPolygonMode(
        get_vk_polygon_mode(init.polygon_mode));

    auto viewport = vk::Viewport{}
                      .setWidth(1920)
                      .setHeight(1080)
                      .setMinDepth(0.f)
                      .setMaxDepth(1.f);

    auto viewport_state = vk::PipelineViewportStateCreateInfo{}
                            .setPViewports(&viewport)
                            .setViewportCount(1)
                            .setScissorCount(1);

    auto multisample_info = vk::PipelineMultisampleStateCreateInfo{};

    auto colour_blend_attachment_state =
      vk::PipelineColorBlendAttachmentState{}
        .setBlendEnable(1)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setAlphaBlendOp(vk::BlendOp::eAdd)
        .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorWriteMask(
          vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eB |
          vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eA);

    if (!init.pshader_spirv.data())
        colour_blend_attachment_state.setColorWriteMask(
          vk::ColorComponentFlags{0});

    auto colour_blend_state =
      vk::PipelineColorBlendStateCreateInfo{}
        .setAttachmentCount(1)
        .setPAttachments(&colour_blend_attachment_state);

    auto depth_stencil_state_info = vk::PipelineDepthStencilStateCreateInfo{}
                                      .setDepthTestEnable(init.do_depth_test)
                                      .setDepthWriteEnable(VK_TRUE)
                                      .setDepthCompareOp(vk::CompareOp::eLess);

    if (init.write_stencil)
    {
        depth_stencil_state_info.setStencilTestEnable(VK_TRUE)
          .setFront(vk::StencilOpState{}
                      .setCompareOp(vk::CompareOp::eAlways)
                      .setPassOp(vk::StencilOp::eReplace)
                      .setDepthFailOp(vk::StencilOp::eReplace)
                      .setFailOp(vk::StencilOp::eReplace)
                      .setCompareMask(0xff)
                      .setWriteMask(0xff)
                      .setReference(1))
          .setBack(depth_stencil_state_info.front);
    }
    if (init.test_stencil)
    {
        depth_stencil_state_info.setStencilTestEnable(VK_TRUE)
          .setFront(vk::StencilOpState{}
                      .setPassOp(vk::StencilOp::eReplace)
                      .setCompareMask(0xff)
                      .setWriteMask(0xff)
                      .setReference(1)
                      .setCompareOp(vk::CompareOp::eNotEqual))
          .setBack(depth_stencil_state_info.front);
    }

    auto pipeline_create_info =
      vk::GraphicsPipelineCreateInfo{}
        .setPStages(stage_create_infos_.data())
        .setStageCount((uint32_t)stage_create_infos_.size())
        .setLayout(pipeline_layout.get())
        .setPVertexInputState(&vertex_input_state_create_info_)
        .setPInputAssemblyState(&input_assembly_state)
        .setPRasterizationState(&rasterisation_state)
        .setPColorBlendState(&colour_blend_state)
        .setRenderPass(render_pass.get())
        .setPDynamicState(&object_dynamic_state_info)
        .setPViewportState(&viewport_state)
        .setPMultisampleState(&multisample_info)
        .setPDepthStencilState(&depth_stencil_state_info);

    auto pipeline = vk_device->createGraphicsPipelineUnique(
      vk::PipelineCache{}, pipeline_create_info);

    return new vulkan_material{
      std::move(descriptor_set_layout),
      std::move(pipeline_layout),
      std::move(pipeline),
      itype_to_vk_itype[(int)init.index_type],
    };
}
} // namespace lmgl
