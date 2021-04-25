#include "VulkanPipeline.h"
#include "Core/Globals.h"
#include "vulkan/vulkan.h"
#include "Vertex.h"
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanResources.h"
#include <cassert>

namespace Ry
{
	VulkanPipeline::VulkanPipeline(const Ry::PipelineCreateInfo& CreateInfo):
	Pipeline2(CreateInfo)
	{

	}
	
	VulkanPipeline::~VulkanPipeline()
	{
		
	}

	bool VulkanPipeline::CreateVertInputBinding(VkPipelineVertexInputStateCreateInfo& OutVertexInputState, Ry::ArrayList<VkVertexInputBindingDescription>& BindingDescriptions)
	{
		// Setup attribute array
		VkVertexInputAttributeDescription* VkAttributes = new VkVertexInputAttributeDescription[CreateInfo.VertFormat.attribute_count];
		
		VkVertexInputBindingDescription BindingDesc;
		BindingDesc.binding = 0;
		BindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		int32 Offset = 0;
		for (int32 AttributeDescIndex = 0; AttributeDescIndex < CreateInfo.VertFormat.attribute_count; AttributeDescIndex++)
		{
			const Ry::VertexAttrib& VertAttrib = CreateInfo.VertFormat.attributes[AttributeDescIndex];

			VkVertexInputAttributeDescription NewAttributeDesc;
			NewAttributeDesc.binding = 0;
			NewAttributeDesc.location = AttributeDescIndex;
			NewAttributeDesc.offset = Offset;

			// Determine format based on size of vertex attribute
			if (VertAttrib.Size == 1)
			{
				NewAttributeDesc.format = VK_FORMAT_R32_SFLOAT;
			}
			else if (VertAttrib.Size == 2)
			{
				NewAttributeDesc.format = VK_FORMAT_R32G32_SFLOAT;
			}
			else if (VertAttrib.Size == 3)
			{
				NewAttributeDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
			}
			else if (VertAttrib.Size == 4)
			{
				NewAttributeDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			else
			{
				Ry::Log->LogErrorf("Unsupported vulkan vertex attribute size: %d", VertAttrib.Size);

				return false;
			}

			Offset += VertAttrib.Size * sizeof(float);

			// Put the attribute description in the array
			VkAttributes[AttributeDescIndex] = NewAttributeDesc;
		}

		// Total offset of vertex buffer is stride.
		BindingDesc.stride = Offset;

		BindingDescriptions.Add(BindingDesc);

		// Setup the vertex input info descriptions
		OutVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		OutVertexInputState.vertexBindingDescriptionCount = BindingDescriptions.GetSize();
		OutVertexInputState.pVertexBindingDescriptions = BindingDescriptions.CopyData();
		OutVertexInputState.vertexAttributeDescriptionCount = CreateInfo.VertFormat.attribute_count;
		OutVertexInputState.pVertexAttributeDescriptions = VkAttributes;

		return true;
	}

	bool VulkanPipeline::CreateBlendState(VkPipelineColorBlendStateCreateInfo& OutBlendState)
	{
		// Map enum blend to vulkan blend factor

		static VkBlendFactor MappedFactors[] = {
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_DST_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO
		};

		static VkBlendOp MappedOps[] = {
			VK_BLEND_OP_ADD
		};

		VkBlendFactor SrcFactor      = MappedFactors[(int32)CreateInfo.Blend.SrcFactor];
		VkBlendFactor DstFactor      = MappedFactors[(int32)CreateInfo.Blend.DstFactor];
		VkBlendFactor SrcAlphaFactor = MappedFactors[(int32)CreateInfo.Blend.SrcAlphaFactor];
		VkBlendFactor DstAlphaFactor = MappedFactors[(int32)CreateInfo.Blend.DstAlphaFactor];
		VkBlendOp     BlendOp        = MappedOps[(int32)CreateInfo.Blend.Op];

		VkPipelineColorBlendAttachmentState ColorBlendAttachment{};
		ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable = CreateInfo.Blend.bEnabled ? VK_TRUE : VK_FALSE;
		ColorBlendAttachment.srcColorBlendFactor = SrcFactor;
		ColorBlendAttachment.dstColorBlendFactor = DstFactor;
		ColorBlendAttachment.colorBlendOp = BlendOp;
		ColorBlendAttachment.srcAlphaBlendFactor = SrcAlphaFactor;
		ColorBlendAttachment.dstAlphaBlendFactor = DstAlphaFactor;
		ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		Ry::ArrayList<VkPipelineColorBlendAttachmentState> ColorBlendAttachments = {
			ColorBlendAttachment
		};
		
		OutBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		OutBlendState.logicOpEnable = VK_FALSE;
		OutBlendState.logicOp = VK_LOGIC_OP_COPY;
		OutBlendState.attachmentCount = ColorBlendAttachments.GetSize();
		OutBlendState.pAttachments = ColorBlendAttachments.CopyData();
		OutBlendState.blendConstants[0] = 0.0f;
		OutBlendState.blendConstants[1] = 0.0f;
		OutBlendState.blendConstants[2] = 0.0f;
		OutBlendState.blendConstants[3] = 0.0f;

		return true;
	}

	bool VulkanPipeline::CreateDynamicState(VkPipelineDynamicStateCreateInfo& DynamicState, Ry::ArrayList<VkDynamicState>& DynamicStates)
	{
		// Dynamic states
		DynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		//DynamicStates = {};
		
		DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicState.dynamicStateCount = DynamicStates.GetSize();
		DynamicState.pDynamicStates = DynamicStates.CopyData();

		return true;
	}

	bool VulkanPipeline::CreateMultisampleState(VkPipelineMultisampleStateCreateInfo& OutInfo)
	{
		OutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		OutInfo.sampleShadingEnable = VK_FALSE;
		OutInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		OutInfo.minSampleShading = 1.0f; // Optional
		OutInfo.pSampleMask = nullptr; // Optional
		OutInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		OutInfo.alphaToOneEnable = VK_FALSE; // Optional

		return true;
	}

	bool VulkanPipeline::CreateRasterizer(VkPipelineRasterizationStateCreateInfo& Rasterizer)
	{
		Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		Rasterizer.depthClampEnable = VK_FALSE;
		Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		Rasterizer.lineWidth = 1.0f;
		Rasterizer.cullMode = VK_CULL_MODE_NONE;
		Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		Rasterizer.depthBiasEnable = VK_FALSE;
		Rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		Rasterizer.depthBiasClamp = 0.0f; // Optional
		Rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		return true;
	}

	bool VulkanPipeline::CreateInputAssembly(VkPipelineInputAssemblyStateCreateInfo& InputAssembly)
	{
		InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssembly.primitiveRestartEnable = VK_FALSE;

		return true;
	}

	bool VulkanPipeline::CreateViewportState(VkPipelineViewportStateCreateInfo& ViewportState, VkViewport& OutViewport, VkRect2D& OutScissor)
	{
		OutViewport.x = 0.0f;
		OutViewport.y = 0.0f;
		OutViewport.width = (float)CreateInfo.ViewportWidth;
		OutViewport.height = (float)CreateInfo.ViewportHeight;
		OutViewport.minDepth = 0.0f;
		OutViewport.maxDepth = 1.0f;

		OutScissor.offset = { 0, 0 };
		OutScissor.extent = VkExtent2D{ CreateInfo.ViewportWidth, CreateInfo.ViewportHeight };

		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.pViewports = &OutViewport;
		ViewportState.scissorCount = 1;
		ViewportState.pScissors = &OutScissor;

		return true;
	}

	bool VulkanPipeline::CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo& OutState)
	{
		OutState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

		// Depth test setting
		OutState.depthTestEnable = CreateInfo.Depth.bEnableDepthTest ? VK_TRUE : VK_FALSE;
		OutState.depthWriteEnable = CreateInfo.Depth.bEnableDepthTest ? VK_TRUE : VK_FALSE;

		OutState.depthCompareOp = VK_COMPARE_OP_LESS;

		OutState.depthBoundsTestEnable = VK_FALSE;
		OutState.minDepthBounds = 0.0f; // Optional
		OutState.maxDepthBounds = 1.0f; // Optional

		OutState.stencilTestEnable = VK_FALSE;
		OutState.front = {}; // Optional
		OutState.back = {}; // Optional

		return true;
	}

	bool VulkanPipeline::CreatePipelineLayout(VkPipelineLayout& OutLayout)
	{

		VkPipelineLayoutCreateInfo VkCreateInfo{};
		VkCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		VkCreateInfo.pushConstantRangeCount = 0; // Optional
		VkCreateInfo.pPushConstantRanges = nullptr; // Optional

		Ry::ArrayList<VkDescriptorSetLayout> Layouts;

		for(ResourceSetDescription* SetDesc : CreateInfo.ResourceDescriptions)
		{
			// Create uniform layout information here
			VulkanResourceSetDescription* VkResSetDesc = dynamic_cast<VulkanResourceSetDescription*>(SetDesc);

			if(VkResSetDesc)
			{
				Layouts.Add(VkResSetDesc->GetVkLayout());
			}
		}

		VkCreateInfo.pSetLayouts = Layouts.CopyData();
		VkCreateInfo.setLayoutCount = Layouts.GetSize();

		if(Layouts.GetSize() <= 0)
		{
			Ry::Log->LogWarn("No resource layouts specified");
		}

		if (vkCreatePipelineLayout(GVulkanContext->GetLogicalDevice(), &VkCreateInfo, nullptr, &OutLayout) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool VulkanPipeline::CreatePipeline()
	{
		VulkanRenderPass* VkRenderPass = dynamic_cast<VulkanRenderPass*>(CreateInfo.RenderPass);
		VulkanShader* VkShader = dynamic_cast<VulkanShader*>(CreateInfo.PipelineShader);

		assert(VkRenderPass != nullptr);
		assert(VkShader != nullptr);

		if(CreateInfo.VertFormat.element_count <= 0)
		{
			Ry::Log->LogError("Vertex format element count is zero");
			return false;
		}

		Ry::ArrayList<VkDynamicState> DynamicStates;
		Ry::ArrayList<VkVertexInputBindingDescription> BindingDescriptions;

		VkPipelineVertexInputStateCreateInfo VkVertInfo{};
		if(!CreateVertInputBinding(VkVertInfo, BindingDescriptions))
		{
			Ry::Log->LogError("Failed to create pipeline vertex state info");
			return false;
		}
		
		VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
		if(!CreateInputAssembly(InputAssembly))
		{
			Ry::Log->LogError("Failed to create input assembly");
			return false;
		}

		VkPipelineViewportStateCreateInfo ViewportState{};
		VkViewport Viewport;
		VkRect2D Scissor;
		if(!CreateViewportState(ViewportState, Viewport, Scissor))
		{
			Ry::Log->LogError("Failed to create viewport state");
			return false;
		}

		VkPipelineRasterizationStateCreateInfo Rasterizer{};
		if(!CreateRasterizer(Rasterizer))
		{
			Ry::Log->LogError("Failed to create rasterizer");
			return false;
		}

		VkPipelineMultisampleStateCreateInfo Multisampling{};
		if(!CreateMultisampleState(Multisampling))
		{
			Ry::Log->LogError("Failed to create multisample state");
			return false;
		}

		VkPipelineColorBlendStateCreateInfo BlendState{};
		if(!CreateBlendState(BlendState))
		{
			Ry::Log->LogError("Failed to create blend state");
			return false;
		}

		VkPipelineDynamicStateCreateInfo DynamicState{};
		if(!CreateDynamicState(DynamicState, DynamicStates))
		{
			Ry::Log->LogError("Failed to create dynamic state");
			return false;
		}

		VkPipelineDepthStencilStateCreateInfo DepthStencil{};
		if(!CreateDepthStencilState(DepthStencil))
		{
			Ry::Log->LogError("Failed to create depth stencil state");
			return false;
		}

		if(!CreatePipelineLayout(PipelineLayout))
		{
			Ry::Log->LogError("Failed to create pipeline layout");
			return false;
		}

		VkPipelineShaderStageCreateInfo ShaderStages[2] = { VkShader->GetVSPipelineCreateInfo(), VkShader->GetFSPipelineCreateInfo() };

		// Entire graphics pipeline depends on shaders? todo: mega oof, i would liberally switch these with opengl
		VkGraphicsPipelineCreateInfo PipelineInfo{};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.pNext = nullptr;
		PipelineInfo.flags = 0;
		PipelineInfo.stageCount = 2;
		PipelineInfo.pStages = ShaderStages;
		PipelineInfo.pVertexInputState = &VkVertInfo;
		PipelineInfo.pInputAssemblyState = &InputAssembly;
		PipelineInfo.pViewportState = &ViewportState;
		PipelineInfo.pRasterizationState = &Rasterizer;
		PipelineInfo.pMultisampleState = &Multisampling;
		PipelineInfo.pDepthStencilState = &DepthStencil;
		PipelineInfo.pColorBlendState = &BlendState;
		PipelineInfo.pDynamicState = &DynamicState;
		PipelineInfo.layout = PipelineLayout;
		PipelineInfo.renderPass = VkRenderPass->GetRenderPass();
		PipelineInfo.subpass = 0;
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional todo: this can be an optimization using a sort of parent/child relationship
		PipelineInfo.basePipelineIndex = -1; // Optional

		// todo: optmization; this function can take in multiple pipeline create infos and cache
		if (vkCreateGraphicsPipelines(GVulkanContext->GetLogicalDevice(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan graphics pipeline");
			return false;
		}
		else
		{
			Ry::Log->Log("Created Vulkan graphics pipeline");
		}

		return true;
	}

	void VulkanPipeline::DeletePipeline()
	{
		vkDestroyPipeline(GVulkanContext->GetLogicalDevice(), GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(GVulkanContext->GetLogicalDevice(), PipelineLayout, nullptr);
	}

}
