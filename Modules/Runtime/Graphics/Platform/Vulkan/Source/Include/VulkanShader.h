#pragma once

#include "Core/Core.h"
#include "Interface2/Shader2.h"
#include "vulkan/vulkan.h"
#include "VulkanGen.h"
#include "Language/ShaderCompiler.h"

namespace Ry
{

	class VULKAN_MODULE VulkanShader : public Ry::Shader2
	{
	public:

		VulkanShader(const AssetRef& VSAsset, const AssetRef& FSAsset);

		VkPipelineShaderStageCreateInfo GetVSPipelineCreateInfo();
		VkPipelineShaderStageCreateInfo GetFSPipelineCreateInfo();

		void DestroyShader() override;
		
	private:


		bool CreateSingleShader(const AssetRef& HLSLSource, Ry::ShaderStage Stage, VkShaderModule& OutModule);

		VkShaderModule VSShaderModule;
		VkShaderModule FSShaderModule;
		VkPipelineShaderStageCreateInfo VSShaderPipeline;
		VkPipelineShaderStageCreateInfo FSShaderPipeline;

	};
	
}
