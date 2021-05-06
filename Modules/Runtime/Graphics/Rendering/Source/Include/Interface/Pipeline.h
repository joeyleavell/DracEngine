#pragma once

#include "Core/Core.h"
#include "Vertex.h"
#include "RenderingGen.h"

namespace Ry
{

	class Shader;
	class RenderPass;
	class SwapChain;
	class UniformBuffer;
	class ResourceLayout;

	struct PipelineDepth
	{
		bool bEnableDepthTest;
	};

	enum class BlendingFactor
	{
		SrcAlpha,
		DstAlpha,
		OneMinusSrcAlpha,
		OneMinusDstAlpha,
		One,
		Zero
	};

	enum class BlendOperator
	{
		Add
	};

	struct PipelineBlendInfo
	{
		// Whether blending should be enabled
		bool bEnabled = false;

		BlendingFactor SrcFactor = Ry::BlendingFactor::SrcAlpha;
		BlendingFactor DstFactor = Ry::BlendingFactor::OneMinusSrcAlpha;
		BlendingFactor SrcAlphaFactor = Ry::BlendingFactor::One;
		BlendingFactor DstAlphaFactor = BlendingFactor::Zero;

		BlendOperator Op = Ry::BlendOperator::Add;

	};

	struct PipelineCreateInfo
	{
		Ry::ArrayList<Ry::ResourceLayout*> ResourceDescriptions;
		Ry::Shader* PipelineShader = nullptr;
		Ry::RenderPass* RenderPass = nullptr;
		Ry::VertexFormat VertFormat;
		PipelineDepth Depth;
		PipelineBlendInfo Blend;

		// todo: GET THESE FROM SWAP CHAIN WHEN CREATING
		uint32 ViewportWidth = 0;
		uint32 ViewportHeight = 0;
	};

	class RENDERING_MODULE Pipeline
	{
	public:
		
		Pipeline(const Ry::PipelineCreateInfo& CreateInfo)
		{
			this->CreateInfo = CreateInfo;
		}
		
		virtual ~Pipeline() = default;

		virtual bool CreatePipeline() = 0;
		virtual void DeletePipeline() = 0;

		const PipelineCreateInfo& GetCreateInfo()
		{
			return CreateInfo;
		}

	protected:

		Ry::PipelineCreateInfo CreateInfo;

	};
	
}
