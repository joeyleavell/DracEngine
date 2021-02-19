#pragma once

#include "Interface2/FrameBuffer2.h"
#include "Core/Core.h"
#include "glew.h"

namespace Ry
{
	class GLTexture;

	class GLFrameBuffer2 : public Ry::FrameBuffer2
	{

	public:

		GLFrameBuffer2(int32 Width, int32 Height, int32 Samples);
		virtual ~GLFrameBuffer2() = default;

		void DeleteFramebuffer() override;

		// todo: figure out how to expose these operations in a generic manner
		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/
		// void Bind() override;
		// void ClearAll(float Red, float Green, float Blue, float Alpha) override;
		// Texture* CreateColorTexture() override;
		// Texture* CreateDepthTexture() override;
		// Texture* CreateStencilTexture() override;
		// void SetColorAttachment(Texture* Texture, int32 AttachPoint) override;
		// void SetDepthAttachment(Texture* Texture) override;
		// void SetStencilAttachment(Texture* Texture) override;
		// void BlitFramebuffer(const FrameBuffer* Dst, bool Color, bool Depth, bool Stencil, int32 ReadColorIndex, int32 WriteColorIndex) override;
		// void ResizeBuffer(int32 IntendedWidth, int32 IntendedHeight) override;
		
		uint32 GetHandle()
		{
			return Handle;
		}
		
	private:
		uint32 Handle;

		//GLenum GLColorAttachments[MAX_COLOR_ATTACHMENTS];

		// GLenum ColorIndexToGLColorAttachment(int32 ColorIndex);
		// void CheckFramebufferStatus();
		
	};
}
