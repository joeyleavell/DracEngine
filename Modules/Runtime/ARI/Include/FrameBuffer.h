#pragma once

#include "Core/Core.h"
#include "RenderPass.h"
#include "FrameBuffer.gen.h"

namespace Ry
{

	class Texture;

	class ARI_MODULE FrameBuffer
	{

	public:

		// todo: this should eventually not be a pointer, currently is to allow vulkan to bypass creation
		FrameBuffer(uint32 Width, uint32 Height, const RenderPass* RenderPass, const FrameBufferDescription* Desc = nullptr);
		virtual ~FrameBuffer() = default;

		virtual void DeleteFramebuffer() = 0;
		virtual void Recreate(uint32 Width, uint32 Height, const RenderPass* NewRenderPass) = 0;
		virtual const ColorAttachment* GetColorAttachment(int32 AttachmentIndex) const = 0;

		uint32 GetIntendedWidth() const
		{
			return IntendedWidth;
		}

		uint32 GetIntendedHeight() const
		{
			return IntendedHeight;
		}

	protected:

		uint32 IntendedWidth;
		uint32 IntendedHeight;
		FrameBufferDescription Description;
	};
	
}
