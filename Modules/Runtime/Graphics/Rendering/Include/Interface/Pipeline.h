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

	struct RENDERING_MODULE RectScissor
	{
		int32 X = -1;
		int32 Y = -1;
		int32 Width = -1;
		int32 Height = -1;

		RectScissor()
		{
						
		}

		RectScissor(int32 InX, int32 InY, int32 InWidth, int32 InHeight)
		{
			this->X = InX;
			this->Y = InY;
			this->Width = InWidth;
			this->Height = InHeight;
		}


		bool IsEnabled() const
		{
			return X >= 0 && Y >= 0 && Width >= 0 && Height >= 0;
		}

		bool operator==(const RectScissor& Other) const
		{
			return X == Other.X && Y == Other.Y && Width == Other.Width && Height == Other.Height;
		}

		bool TestAgainst(float OtherX, float OtherY)
		{
			return (OtherX >= X && OtherX < X + Width) && (OtherY >= Y && OtherY < Y + Height);
		}

		bool TestAgainst(const RectScissor& Other)
		{
			bool bVisX = Other.X >= X && Other.X < X + Width;
			bVisX |= Other.X + Other.Width >= X && Other.X + Other.Width < X + Width;
			bVisX |= Other.X < X && Other.X + Other.Width >= X + Width;

			bool bVisY = Other.Y >= Y && Other.Y < Y + Height;
			bVisY |= Other.Y + Other.Height >= Y && Other.Y + Other.Height < Y + Height;
			bVisY |= Other.Y < Y && Other.Y + Other.Height >= Y + Height;

			return bVisX && bVisY;
		}
	};

	struct PipelineCreateInfo
	{
		Ry::ArrayList<Ry::ResourceLayout*> ResourceDescriptions;
		Ry::Shader* PipelineShader = nullptr;
		Ry::RenderPass* RenderPass = nullptr;
		Ry::VertexFormat VertFormat;
		PipelineDepth Depth;
		PipelineBlendInfo Blend;

		bool bEnableScissorTest = true;
		RectScissor Scissor;

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
