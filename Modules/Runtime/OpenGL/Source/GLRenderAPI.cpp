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
#include "GLFrameBuffer.h"
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

	CommandBuffer* GLRenderAPI::CreateCommandBuffer(RenderPass* ParentRenderPass)
	{
		if(ParentRenderPass)
		{
			return new GLCommandBuffer(SecondaryCommandBufferInfo{true, ParentRenderPass});
		}
		else
		{
			return new GLCommandBuffer;
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

	Texture* GLRenderAPI::CreateTexture(TextureFiltering Filter)
	{
		return new GLTexture(TextureUsage::STATIC, Filter);
	}

	RenderPass* GLRenderAPI::CreateRenderPass()
	{
		return new GLRenderPass;
	}

	FrameBuffer* GLRenderAPI::CreateFrameBuffer(int32 Width, int32 Height, const RenderPass* RenderPass, const FrameBufferDescription& Description)
	{
		return new GLFrameBuffer(Width, Height, RenderPass, &Description);
	}

	bool InitOGLRendering()
	{
		RendAPI = new GLRenderAPI;

		return true;
	}

}
