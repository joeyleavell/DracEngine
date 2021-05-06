
#include "VulkanCommandBuffer.h"
#include "VulkanRenderAPI.h"
#include "Core/Globals.h"
#include "Interface/VertexArray.h"
#include "VulkanVertexArray.h"
#include "Interface/Shader.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanResources.h"
#include "VulkanTexture.h"
#include "VulkanContext.h"
#include "Interface/Texture.h"

namespace Ry
{

	CommandBuffer* VulkanRenderAPI::CreateCommandBuffer(Ry::SwapChain* Target, RenderPass* ParentRenderPass)
	{
		if(ParentRenderPass)
		{
			return new Ry::VulkanCommandBuffer2(Target, SecondaryCommandBufferInfo{true, ParentRenderPass});
		}
		else
		{
			return new Ry::VulkanCommandBuffer2(Target);
		}
	}

	VertexArray* VulkanRenderAPI::CreateVertexArray(const Ry::VertexFormat& Format)
	{
		return new Ry::VulkanVertexArray(Format);
	}

	Shader* VulkanRenderAPI::CreateShader(Ry::String VertexLoc, Ry::String FragmentLoc)
	{
		VulkanShader* Shad = new Ry::VulkanShader(VertexLoc, FragmentLoc);
		return Shad;
	}

	Pipeline* VulkanRenderAPI::CreatePipeline(const PipelineCreateInfo& CreateInfo)
	{
		VulkanPipeline* Pipeline = new VulkanPipeline(CreateInfo);
		return Pipeline;
	}

	ResourceLayout* VulkanRenderAPI::CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex)
	{
		VulkanResourceSetDescription* ResSet = new VulkanResourceSetDescription(Stages, SetIndex);
		return ResSet;
	}

	ResourceSet* VulkanRenderAPI::CreateResourceSet(const ResourceLayout* Desc, SwapChain* SC)
	{
		VulkanResourceSet* ResSet = new VulkanResourceSet(Desc, SC);
		return ResSet;
	}

	Texture* VulkanRenderAPI::CreateTexture()
	{
		return new Ry::VulkanTexture(Ry::TextureUsage::STATIC);
	}

	bool InitVulkanAPI()
	{
		// Create a new Vulkan Render API and assign it to globals
		Ry::RendAPI = new VulkanRenderAPI;

		// Create the vulkan context
		GVulkanContext = new VulkanContext;
		if(!GVulkanContext->InitContext())
		{
			return false;
		}

		return true;
	}

	void ShutdownVulkanAPI()
	{
		GVulkanContext->CleanupContext();
		delete GVulkanContext;
	}
}
