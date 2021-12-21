#include "GLFrameBuffer.h"
#include "GLTexture.h"

namespace Ry
{
	GLFrameBuffer::GLFrameBuffer(int32 Width, int32 Height, const RenderPass* RenderPass, const FrameBufferDescription* Desc):
	FrameBuffer(Width, Height, RenderPass, Desc)
	{
		// Determine if this framebuffer is just an alias for the default fbo
		bool bHasAnyNonSwapColor = false;
		bool bHasAnySwapColor    = false;
		bool bHasAnyNonSwapDepth = false;
		bool bHasAnySwapDepth    = false;

		bDefaultFboAlias = false;

		for (const AttachmentDescription& AttachDesc : Description.Attachments)
		{
			if(AttachDesc.ReferencingSwapChain)
			{
				if (AttachDesc.Format == AttachmentFormat::Color)
					bHasAnySwapColor = true;
				else
					bHasAnySwapDepth = true;
			}
			else
			{
				if (AttachDesc.Format == AttachmentFormat::Color)
					bHasAnyNonSwapColor = true;
				else
					bHasAnySwapDepth = true;
			}
		}

		if(bHasAnySwapColor && bHasAnyNonSwapColor || 
			bHasAnySwapDepth && bHasAnyNonSwapDepth ||
			bHasAnySwapColor && bHasAnyNonSwapDepth ||
			bHasAnyNonSwapColor && bHasAnySwapDepth)
		{
			Ry::Log->LogErrorf("GLFrameBuffer: Can't mix swap-based attachments with non-swap based attachments, treating this as default FBO alias");
			bDefaultFboAlias = true;
		}
		else if(bHasAnySwapColor)
		{
			// This is an alias for the default framebuffer
			bDefaultFboAlias = true;
		}

		if(!bDefaultFboAlias)
		{
			CreateFramebuffer(Width, Height);
		}
	}

	void GLFrameBuffer::DeleteFramebuffer()
	{

		// Delete all color attachments
		for (const GLColorAttachment* GLColor : ColorAttachments)
		{
			glDeleteTextures(1, &GLColor->AttachmentHandle);
		}

		// Free framebuffer resource
		glDeleteFramebuffers(1, &Handle);

	}

	void GLFrameBuffer::CreateFramebuffer(uint32 Width, uint32 Height)
	{
		// Generate GL framebuffer handle
		glGenFramebuffers(1, &Handle);

		// Bind framebufer
		glBindFramebuffer(GL_FRAMEBUFFER, Handle);

		// Create attachments
		int32 CurrentColorAttachment = 0;
		for (const AttachmentDescription& AttachDesc : Description.Attachments)
		{
			// Don't create attachments if we rely on 
			if (AttachDesc.ReferencingSwapChain || AttachDesc.ExistingAttachment)
				continue;

			if (AttachDesc.Format == AttachmentFormat::Color)
			{
				// Create new attachment as texture
				// TODO: For depth and stencil attachments, these should probably be created as renderbuffer objects as an optimization					
				GLColorAttachment* NewColorAttachment = new GLColorAttachment;
				glGenTextures(1, &NewColorAttachment->AttachmentHandle);

				glBindTexture(GL_TEXTURE_2D, NewColorAttachment->AttachmentHandle);
				{
					// Set texture filters to linear
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					// Color attachments use the RGB format
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

					// Determine attach index
					GLuint AttachIndex = GL_COLOR_ATTACHMENT0 + CurrentColorAttachment;

					// Attach texture to framebuffer
					glFramebufferTexture2D(GL_FRAMEBUFFER, AttachIndex, GL_TEXTURE_2D, NewColorAttachment->AttachmentHandle, 0);

					// Save the created attachment for usage later
					ColorAttachments.Add(NewColorAttachment);
					CurrentColorAttachment++;
				}
			}

		}

		if (Description.bHasDepthAttachment || Description.bHasStencilAttachment)
		{
			// Depth or stencil enabled always means depth+stencil in OpenGL
			GLuint DepthStencilHandle;
			glGenTextures(1, &DepthStencilHandle);

			glBindTexture(GL_TEXTURE_2D, DepthStencilHandle);
			{
				// Set texture filters to linear
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Width, Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

				// Bind the stencil attachment
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilHandle, 0);
			}

			DepthStencilAttachment = DepthStencilHandle;
		}

		// Check status is valid
		GLint Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "GL frame buffer " << Handle << " is not complete " << Status << std::endl;
		}

	}

	void GLFrameBuffer::Recreate(uint32 Width, uint32 Height, const RenderPass* NewRenderPass)
	{
		if(!bDefaultFboAlias)
		{
			// Re-create framebuffer with new size
			DeleteFramebuffer();
			CreateFramebuffer(Width, Height);
		}
	}

	const ColorAttachment* GLFrameBuffer::GetColorAttachment(int32 AttachmentIndex) const
	{
		return ColorAttachments[AttachmentIndex];
	}

	uint32 GLFrameBuffer::GetHandle()
	{
		return Handle;
	}

	bool GLFrameBuffer::HasDepthStencilAttachment() const
	{
		return bHasDepthStencilAttachment;
	}

	GLuint GLFrameBuffer::GetDepthStencilAttachment() const
	{
		return DepthStencilAttachment;
	}

	bool GLFrameBuffer::IsDefaultAlias() const
	{
		return bDefaultFboAlias;
	}

}
