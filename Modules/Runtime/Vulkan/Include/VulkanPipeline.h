#pragma once

#include "Core/Core.h"
#include "VulkanCommon.h"
#include "Interface/Pipeline.h"
#include "VulkanGen.h"

namespace Ry
{
	class VulkanSwapChain;
	class VulkanShader;
	class VulkanVertexArray;
	class VulkanBuffer;

	class VULKAN_MODULE VulkanPipeline : public Ry::Pipeline
	{
	public:
		VulkanPipeline(const Ry::PipelineCreateInfo& CreateInfo);
		~VulkanPipeline();

		VkPipelineLayout PipelineLayout;
		VkPipeline GraphicsPipeline;
		
		bool CreatePipeline() override;
		void DeletePipeline() override;

	private:

		bool CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo& OutState);
		bool CreatePipelineLayout(VkPipelineLayout& OutLayout);
		bool CreateVertInputBinding(VkPipelineVertexInputStateCreateInfo& OutVertexInputState, Ry::ArrayList<VkVertexInputBindingDescription>& BindingDescriptions);
		bool CreateBlendState(VkPipelineColorBlendStateCreateInfo& OutBlendState);
		bool CreateDynamicState(VkPipelineDynamicStateCreateInfo& DynamicState, Ry::ArrayList<VkDynamicState>& DynamicStates);
		bool CreateMultisampleState(VkPipelineMultisampleStateCreateInfo& OutInfo);
		bool CreateRasterizer(VkPipelineRasterizationStateCreateInfo& Rasterizer);
		bool CreateInputAssembly(VkPipelineInputAssemblyStateCreateInfo& InputAssembly);
		bool CreateViewportState(VkPipelineViewportStateCreateInfo& ViewportState, VkViewport& OutViewport, VkRect2D& OutScissor);

	};
	
}
