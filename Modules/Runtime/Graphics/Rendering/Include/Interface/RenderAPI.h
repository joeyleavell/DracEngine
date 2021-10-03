#pragma once

#include "RenderingGen.h"
#include "Core/Core.h"

namespace Ry
{
	class VertexFormat;

	class CommandBuffer;
	class SwapChain;
	class VertexArray;
	class Shader;
	class AssetRef;
	class Pipeline;
	class ResourceLayout;
	class ResourceSet;
	class Texture;
	class RenderPass;
	struct PipelineCreateInfo;
	enum class ShaderStage;
	enum class TextureFiltering;

	class RENDERING_MODULE RenderAPI
	{
	public:

		RenderAPI() {};
		virtual ~RenderAPI() = default;

		virtual CommandBuffer* CreateCommandBuffer(Ry::SwapChain* Target, RenderPass* ParentRenderPass = nullptr) = 0;
		virtual VertexArray* CreateVertexArray(const Ry::VertexFormat& Format) = 0;
		virtual Shader* CreateShader(Ry::String VertexShaderLoc, Ry::String FragShaderLoc) = 0;
		virtual Pipeline* CreatePipeline(const PipelineCreateInfo& CreateInfo) = 0;
		virtual ResourceLayout* CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0) = 0;
		virtual ResourceSet* CreateResourceSet(const ResourceLayout* Desc, SwapChain* SC) = 0;
		virtual Texture* CreateTexture(TextureFiltering Filter) = 0;

		virtual Pipeline* CreatePipelineFromShader(const PipelineCreateInfo& CreateInfo, Shader* Src);


	};
	
}