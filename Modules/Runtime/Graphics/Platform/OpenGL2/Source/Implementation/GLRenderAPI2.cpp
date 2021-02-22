#include "Core/Globals.h"
#include "GLRenderAPI2.h"
#include "Asset.h"
#include "glew.h"
#include "GLCommandBuffer2.h"
#include "GLVertexArray2.h"
#include "GLShader2.h"
#include "GLRenderingState2.h"
#include "GLResources2.h"
#include "GLTexture2.h"

namespace Ry
{
	RenderingCommandBuffer2* GLRenderAPI::CreateCommandBuffer(Ry::SwapChain* Target)
	{
		return new GLCommandBuffer2(Target);
	}

	VertexArray2* GLRenderAPI::CreateVertexArray(const Ry::VertexFormat& Format)
	{
		return new GLVertexArray2(Format);
	}

	Shader2* GLRenderAPI::CreateShader(Ry::String VSAsset, Ry::String FSAsset)
	{
		return new GLShader2(VSAsset, FSAsset);
	}

	Pipeline2* GLRenderAPI::CreatePipeline(const PipelineCreateInfo& CreateInfo)
	{
		return new GLState(CreateInfo);
	}

	ResourceSetDescription* GLRenderAPI::CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages,
		int32 SetIndex)
	{
		return new GLResourceDescription2(Stages, SetIndex);
	}

	ResourceSet* GLRenderAPI::CreateResourceSet(ResourceSetDescription* Desc, SwapChain* SC)
	{
		return new GLResources2(Desc, SC);
	}

	Texture2* GLRenderAPI::CreateTexture()
	{
		return new GLTexture2(TextureUsage::STATIC);
	}

	bool InitOGLRendering()
	{
		NewRenderAPI = new GLRenderAPI;

		return true;
	}

}
