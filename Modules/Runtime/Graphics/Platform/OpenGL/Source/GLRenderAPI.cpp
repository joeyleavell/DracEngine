#include "Core/Globals.h"
#include "GLRenderAPI.h"
#include "Asset.h"
#include "glew.h"
#include "GLCommandBuffer.h"
#include "GLVertexArray.h"
#include "GLShader.h"
#include "GLRenderingState.h"
#include "GLResources.h"
#include "GLTexture.h"
#include "Interface/Texture.h"

namespace Ry
{
	CommandBuffer* GLRenderAPI::CreateCommandBuffer(Ry::SwapChain* Target, RenderPass* ParentRenderPass)
	{
		if(ParentRenderPass)
		{
			return new GLCommandBuffer(Target, SecondaryCommandBufferInfo{true, ParentRenderPass});
		}
		else
		{
			return new GLCommandBuffer(Target);
		}
	}

	VertexArray* GLRenderAPI::CreateVertexArray(const Ry::VertexFormat& Format)
	{
		return new GLVertexArray(Format);
	}

	Shader* GLRenderAPI::CreateShader(Ry::String VSAsset, Ry::String FSAsset)
	{
		return new GLShader(VSAsset, FSAsset);
	}

	Pipeline* GLRenderAPI::CreatePipeline(const PipelineCreateInfo& CreateInfo)
	{
		return new GLState(CreateInfo);
	}

	ResourceLayout* GLRenderAPI::CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages,
		int32 SetIndex)
	{
		return new GLResourceDescription(Stages, SetIndex);
	}

	ResourceSet* GLRenderAPI::CreateResourceSet(const ResourceLayout* Desc, SwapChain* SC)
	{
		return new GLResources(Desc, SC);
	}

	Texture* GLRenderAPI::CreateTexture()
	{
		return new GLTexture(TextureUsage::STATIC);
	}

	bool InitOGLRendering()
	{
		RendAPI = new GLRenderAPI;

		return true;
	}

}
