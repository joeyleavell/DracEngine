#pragma once

#include "Interface/RenderAPI.h"
#include "OpenGLGen.h"

namespace Ry
{

	/**
	 * OpenGL implementation of rendering interface.
	 */
	class OPENGL_MODULE GLRenderAPI : public RenderAPI
	{
	public:

		GLRenderAPI() {};
		virtual ~GLRenderAPI() = default;

		CommandBuffer* CreateCommandBuffer(Ry::SwapChain* Target, RenderPass* ParentRenderPass = nullptr);
		VertexArray* CreateVertexArray(const Ry::VertexFormat& Format);
		Shader* CreateShader(Ry::String VSAsset, Ry::String FSAsset);
		Pipeline* CreatePipeline(const PipelineCreateInfo& CreateInfo);
		ResourceLayout* CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0);
		ResourceSet* CreateResourceSet(const ResourceLayout* Desc, SwapChain* SC);
		Texture* CreateTexture(TextureFiltering Filter);

	private:

	};


	OPENGL_MODULE bool InitOGLRendering();
	

}