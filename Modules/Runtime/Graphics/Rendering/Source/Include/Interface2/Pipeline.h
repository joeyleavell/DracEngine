#pragma once

#include "Core/Core.h"
#include "Vertex.h"
#include "RenderingGen.h"

namespace Ry
{

	class Shader2;
	class RenderPass2;
	class SwapChain;
	class UniformBuffer;
	class ResourceSetDescription;

	struct PipelineCreateInfo
	{
		Ry::ArrayList<Ry::ResourceSetDescription*> ResourceDescriptions;
		Ry::Shader2* PipelineShader = nullptr;
		Ry::RenderPass2* RenderPass = nullptr;
		Ry::VertexFormat VertFormat;

		// todo: GET THESE FROM SWAP CHAIN WHEN CREATING
		uint32 ViewportWidth = 0;
		uint32 ViewportHeight = 0;
	};

	class RENDERING_MODULE Pipeline2
	{
	public:
		
		Pipeline2(const Ry::PipelineCreateInfo& CreateInfo)
		{
			this->CreateInfo = CreateInfo;
		}
		
		virtual ~Pipeline2() = default;

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
