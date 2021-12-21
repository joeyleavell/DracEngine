#pragma once

#include "Interface/FrameBuffer.h"
#include "Core/Core.h"
#include "glew.h"

namespace Ry
{
	class GLTexture;

	class GLFrameBuffer : public Ry::FrameBuffer
	{

	public:

		GLFrameBuffer(int32 Width, int32 Height, const RenderPass* RenderPass, const FrameBufferDescription* Desc);
		virtual ~GLFrameBuffer() = default;
		void DeleteFramebuffer() override;
		void Recreate(uint32 Width, uint32 Height, const RenderPass* NewRenderPass) override;
		const ColorAttachment* GetColorAttachment(int32 AttachmentIndex) const override;

		uint32 GetHandle();
		GLuint GetColorAttachmentTexture(int32 AttachmentIndex) const;
		bool HasDepthStencilAttachment() const;
		GLuint GetDepthStencilAttachment() const;

	private:
		uint32 Handle;

		Ry::ArrayList<GLuint> ColorAttachments;

		GLuint DepthStencilAttachment;
		bool bHasDepthStencilAttachment = false;

	};
}
