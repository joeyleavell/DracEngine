#pragma once

#include "Core/Core.h"
#include "VulkanCommon.h"
#include "Interface2/Pipeline.h"
#include "VulkanGen.h"

namespace Ry
{
	class VulkanSwapChain;
	class VulkanShader;
	class VulkanVertexArray;
	class VulkanBuffer;
	
	// enum UniformDataType
	// {
	// 	Float,
	// 	Float2,
	// 	Float3,
	// 	Float4,
	// 	Mat2x2,
	// 	Mat3x3,
	// 	Mat4x4
	// };
	//
	// struct UniformType
	// {
	// 	UniformDataType DataType;
	// 	int32 SizeBytes;
	// };
	//
	// class UniformDescription
	// {
	//
	// private:
	// 	
	// };
	//
	// class VULKAN_MODULE Pipeline
	// {
	//
	// private:
	//
	// 	Ry::Shader2* Shader;
	// };

	class VULKAN_MODULE VulkanPipeline : public Ry::Pipeline2
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
