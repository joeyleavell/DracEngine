#pragma once

#include "Interface2/RenderAPI.h"
#include "VulkanGen.h"

namespace Ry
{

	class Framebuffer2;
	class SwapChain;
	class Shader2;
	class AssetRef;
	
	class VULKAN_MODULE VulkanRenderAPI : public Ry::RenderAPI2
	{
	public:

		RenderingCommandBuffer2* CreateCommandBuffer(Ry::SwapChain* Target, RenderPass2* ParentRenderPass = nullptr) override;
		VertexArray2* CreateVertexArray(const Ry::VertexFormat& Format) override;
		Shader2* CreateShader(Ry::String VertexLoc, Ry::String FragmentLoc) override;
		Pipeline2* CreatePipeline(const PipelineCreateInfo& CreateInfo) override;
		ResourceSetDescription* CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0) override;
		ResourceSet* CreateResourceSet(const ResourceSetDescription* Desc, SwapChain* SC) override;
		Texture2* CreateTexture() override;


	};

	extern VULKAN_MODULE bool InitVulkanAPI();
	extern VULKAN_MODULE void ShutdownVulkanAPI();

}
