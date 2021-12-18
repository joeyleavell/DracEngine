#pragma once

#include "Interface/RenderAPI.h"
#include "VulkanGen.h"

namespace Ry
{

	class Framebuffer2;
	class SwapChain;
	class Shader;
	class AssetRef;
	
	class VULKAN_MODULE VulkanRenderAPI : public Ry::RenderAPI
	{
	public:

		CommandBuffer* CreateCommandBuffer(Ry::SwapChain* Target, RenderPass* ParentRenderPass = nullptr) override;
		VertexArray* CreateVertexArray(const Ry::VertexFormat& Format) override;
		Shader* CreateShader(Ry::String VertexLoc, Ry::String FragmentLoc) override;
		Pipeline* CreatePipeline(const PipelineCreateInfo& CreateInfo) override;
		ResourceLayout* CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0) override;
		ResourceSet* CreateResourceSet(const ResourceLayout* Desc, SwapChain* SC) override;
		Texture* CreateTexture(TextureFiltering Filter) override;
		RenderPass* CreateRenderPass() override;


	};

	extern VULKAN_MODULE bool InitVulkanAPI();
	extern VULKAN_MODULE void ShutdownVulkanAPI();

}
