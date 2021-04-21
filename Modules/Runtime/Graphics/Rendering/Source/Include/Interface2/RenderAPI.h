#pragma once

#include "RenderingGen.h"
#include "Core/Core.h"

namespace Ry
{
	struct VertexFormat;

	class RenderingCommandBuffer2;
	class SwapChain;
	class VertexArray2;
	class Shader2;
	class AssetRef;
	class Pipeline2;
	class ResourceSetDescription;
	class ResourceSet;
	class Texture2;
	class RenderPass2;
	struct PipelineCreateInfo;
	enum class ShaderStage;

	class RENDERING_MODULE RenderAPI2
	{
	public:

		RenderAPI2() {};
		virtual ~RenderAPI2() = default;

		virtual RenderingCommandBuffer2* CreateCommandBuffer(Ry::SwapChain* Target, RenderPass2* ParentRenderPass = nullptr) = 0;
		virtual VertexArray2* CreateVertexArray(const Ry::VertexFormat& Format) = 0;
		virtual Shader2* CreateShader(Ry::String VertexShaderLoc, Ry::String FragShaderLoc) = 0;
		virtual Pipeline2* CreatePipeline(const PipelineCreateInfo& CreateInfo) = 0;
		virtual ResourceSetDescription* CreateResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0) = 0;
		virtual ResourceSet* CreateResourceSet(ResourceSetDescription* Desc, SwapChain* SC) = 0;
		virtual Texture2* CreateTexture() = 0;

	};
	
}