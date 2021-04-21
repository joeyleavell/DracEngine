#pragma once

#include "Interface2/RenderAPI.h"
#include "OpenGL2Gen.h"

namespace Ry
{

	/**
	 * OpenGL implementation of rendering interface.
	 */
	class OPENGL2_MODULE GLRenderAPI : public RenderAPI2
	{
	public:

		GLRenderAPI() {};
		virtual ~GLRenderAPI() = default;

		RenderingCommandBuffer2* CreateCommandBuffer(Ry::SwapChain* Target, RenderPass2* ParentRenderPass = nullptr);
		VertexArray2* CreateVertexArray(const Ry::VertexFormat& Format);
		Shader2* CreateShader(Ry::String VSAsset, Ry::String FSAsset);
		Pipeline2* CreatePipeline(const PipelineCreateInfo& CreateInfo);
		ResourceSetDescription* CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0);
		ResourceSet* CreateResourceSet(ResourceSetDescription* Desc, SwapChain* SC);
		Texture2* CreateTexture();

	private:

	};


	OPENGL2_MODULE bool InitOGLRendering();
	

}