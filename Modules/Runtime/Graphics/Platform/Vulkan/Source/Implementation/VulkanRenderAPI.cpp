
#include "VulkanCommandBuffer.h"
#include "VulkanRenderAPI.h"
#include "Core/Globals.h"
#include "Interface2/VertexArray2.h"
#include "VulkanVertexArray.h"
#include "Interface2/Shader2.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanResources.h"
#include "VulkanTexture.h"
#include "VulkanContext.h"

namespace Ry
{

	RenderingCommandBuffer2* VulkanRenderAPI::CreateCommandBuffer(Ry::SwapChain* Target, RenderPass2* ParentRenderPass)
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

	VertexArray2* VulkanRenderAPI::CreateVertexArray(const Ry::VertexFormat& Format)
	{
		return new Ry::VulkanVertexArray(Format);
	}

	Shader2* VulkanRenderAPI::CreateShader(Ry::String VertexLoc, Ry::String FragmentLoc)
	{
		VulkanShader* Shad = new Ry::VulkanShader(VertexLoc, FragmentLoc);
		return Shad;
	}

	Pipeline2* VulkanRenderAPI::CreatePipeline(const PipelineCreateInfo& CreateInfo)
	{
		VulkanPipeline* Pipeline = new VulkanPipeline(CreateInfo);
		return Pipeline;
	}

	ResourceSetDescription* VulkanRenderAPI::CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex)
	{
		VulkanResourceSetDescription* ResSet = new VulkanResourceSetDescription(Stages, SetIndex);
		return ResSet;
	}

	ResourceSet* VulkanRenderAPI::CreateResourceSet(ResourceSetDescription* Desc, SwapChain* SC)
	{
		VulkanResourceSet* ResSet = new VulkanResourceSet(Desc, SC);
		return ResSet;
	}

	Texture2* VulkanRenderAPI::CreateTexture()
	{
		return new Ry::VulkanTexture(Ry::TextureUsage::STATIC);
	}

	bool InitVulkanAPI()
	{
		// Create a new Vulkan Render API and assign it to globals
		Ry::NewRenderAPI = new VulkanRenderAPI;

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
