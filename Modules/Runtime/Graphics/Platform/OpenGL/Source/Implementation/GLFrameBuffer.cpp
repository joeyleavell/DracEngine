#include "GLFrameBuffer.h"
#include "GLTexture.h"

namespace Ry
{
	GLFrameBuffer::GLFrameBuffer(int32 Width, int32 Height, int32 Samples):
	FrameBuffer(Width, Height, Samples)
	{
		// Generate GL framebuffer handle
		glGenFramebuffers(1, &Handle);

		// Initialize color attachment count to zero
		this->ColorAttachmentCount = 0;
	}

	void GLFrameBuffer::DeleteFramebuffer()
	{
		// Free framebuffer resource
		glDeleteFramebuffers(1, &Handle);
	}

	// void GLFrameBuffer::Bind()
	// {
	// 	glBindFramebuffer(GL_FRAMEBUFFER, Handle);
	// }
	//
	// void GLFrameBuffer::ClearAll(float Red, float Green, float Blue, float Alpha)
	// {
	// 	glClearColor(Red, Green, Blue, Alpha);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// }
	//
	// void GLFrameBuffer::SetColorAttachment(Texture* Texture, int32 AttachPoint)
	// {		
	// 	GLenum GLColorAttachment = ColorIndexToGLColorAttachment(AttachPoint);
	// 	auto* GLTexture = dynamic_cast<Ry::GLTexture*>(Texture);
	//
	// 	if (GLTexture)
	// 	{
	// 		// Bind this frame buffer
	// 		Bind();
	//
	// 		// Only register an additional attachment point if this is a unique one
	// 		if (GetColorAttachment(AttachPoint) == nullptr)
	// 		{
	// 			// Override the current attach point
	// 			ColorAttachments[ColorAttachmentCount] = GLTexture;
	// 			GLColorAttachments[ColorAttachmentCount] = GLColorAttachment;
	// 			ColorAttachmentCount++;
	// 		}
	//
	// 		glFramebufferTexture2D(GL_FRAMEBUFFER, GLColorAttachment, GLTexture->GetTarget(), GLTexture->GetHandle(), 0);
	//
	// 		// Update the draw buffers
	// 		glDrawBuffers(ColorAttachmentCount, GLColorAttachments);
	//
	// 		// Check the status of the framebuffer
	// 		CheckFramebufferStatus();
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "Only GL textures can be used for GL frame buffers." << std::endl;
	// 	}
	// }
	//
	// void GLFrameBuffer::SetDepthAttachment(Texture* Texture)
	// {		
	// 	auto* GLTexture = dynamic_cast<Ry::GLTexture*>(Texture);
	// 	Bind();
	//
	// 	if (GLTexture)
	// 	{
	// 		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GLTexture->GetTarget(), GLTexture->GetHandle(), 0);
	//
	// 		DepthAttachment = Texture;
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "Only GL textures can be used for GL frame buffers." << std::endl;
	// 	}
	//
	// 	// Check the status of the framebuffer
	// 	CheckFramebufferStatus();
	// }
	//
	// void GLFrameBuffer::SetStencilAttachment(Texture* Texture)
	// {
	// 	auto* GLTexture = dynamic_cast<Ry::GLTexture*>(Texture);
	// 	Bind();
	//
	// 	if (GLTexture)
	// 	{
	// 		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, GLTexture->GetHandle(), 0);
	//
	// 		StencilAttachment = Texture;
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "Only GL textures can be used for GL frame buffers." << std::endl;
	// 	}
	//
	// 	// Check the status of the framebuffer
	// 	CheckFramebufferStatus();
	// }
	//
	// void GLFrameBuffer::BlitFramebuffer(const FrameBuffer* Dst, bool Color, bool Depth, bool Stencil, int32 ReadColorIndex, int32 WriteColorIndex)
	// {
	// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, Handle);
	//
	// 	glReadBuffer(ColorIndexToGLColorAttachment(ReadColorIndex));
	// 	
	// 	int32 Mask = 0;
	// 	if (Color)
	// 	{
	// 		Mask |= GL_COLOR_BUFFER_BIT;
	// 	}
	// 	if(Depth)
	// 	{
	// 		Mask |= GL_DEPTH_BUFFER_BIT;
	// 	}
	// 	if(Stencil)
	// 	{
	// 		Mask |= GL_STENCIL_BUFFER_BIT;
	// 	}
	//
	// 	if (Dst)
	// 	{
	// 		const GLFrameBuffer* GLDst = dynamic_cast<const GLFrameBuffer*>(Dst);
	// 		if(!GLDst)
	// 		{
	// 			std::cerr << "Passed in a frame buffer other than a GL frame buffer to BlitFrameBuffer" << std::endl;
	// 			return;
	// 		}
	// 		
	// 		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLDst->Handle);
	// 		glDrawBuffer(ColorIndexToGLColorAttachment(WriteColorIndex));
	//
	// 		glBlitFramebuffer(0, 0, IntendedWidth, IntendedHeight, 0, 0, Dst->GetIntendedWidth(), Dst->GetIntendedHeight(), Mask, GL_NEAREST);
	// 	}
	// 	else
	// 	{
	// 		// Use default screen as draw buffer
	// 		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// 		glDrawBuffer(GL_BACK);
	//
	// 		glBlitFramebuffer(0, 0, IntendedWidth, IntendedHeight, 0, 0, IntendedWidth, IntendedHeight, Mask, GL_NEAREST);
	// 	}
	//
	// }
	//
	// void GLFrameBuffer::ResizeBuffer(int32 IntendedWidth, int32 IntendedHeight)
	// {
	// 	FrameBuffer::ResizeBuffer(IntendedWidth, IntendedHeight);
	//
	//
	// 	
	// }
	//
	// Texture* GLFrameBuffer::CreateColorTexture()
	// {
	// 	GLTexture* Result = nullptr;
	// 	if (Samples > 0)
	// 	{
	// 		Result = new Ry::GLTextureMultisample(IntendedWidth, IntendedHeight, Samples, GL_RGBA);
	// 	}
	// 	else
	// 	{
	// 		Result = new Ry::GLTexture;
	// 		Result->AllocateColor(IntendedWidth, IntendedHeight);
	// 	}
	//
	// 	return Result;
	// }
	//
	// Texture* GLFrameBuffer::CreateDepthTexture()
	// {
	// 	GLTexture* Result = nullptr;
	// 	if (Samples > 0)
	// 	{
	// 		Result = new Ry::GLTextureMultisample(IntendedWidth, IntendedHeight, Samples, GL_DEPTH_COMPONENT24);
	// 	}
	// 	else
	// 	{
	// 		Result = new Ry::GLTexture;
	// 		Result->AllocateDepth(IntendedWidth, IntendedHeight);
	// 	}
	//
	// 	return Result;
	// }
	//
	// Texture* GLFrameBuffer::CreateStencilTexture()
	// {
	// 	std::cerr << "OpenGL stencil buffer textures are currently not supported" << std::endl;
	//
	// 	return nullptr;
	// 	//return new Ry::GLTexture(GL_TEXTURE_2D, GL_STENCIL_COM);
	// }
	//
	// void GLFrameBuffer::CheckFramebufferStatus()
	// {
	// 	GLint Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	// 	if (Status != GL_FRAMEBUFFER_COMPLETE)
	// 	{
	// 		std::cerr << "GL frame buffer " << Handle << " is not complete " << Status << std::endl;
	// 	}
	// }
	//
	// GLenum GLFrameBuffer::ColorIndexToGLColorAttachment(int32 ColorIndex)
	// {
	// 	switch (ColorIndex)
	// 	{
	// 	case 0:
	// 		return GL_COLOR_ATTACHMENT0;
	// 	case 1:
	// 		return GL_COLOR_ATTACHMENT1;
	// 	case 2:
	// 		return GL_COLOR_ATTACHMENT2;
	// 	case 3:
	// 		return GL_COLOR_ATTACHMENT3;
	// 	case 4:
	// 		return GL_COLOR_ATTACHMENT4;
	// 	case 5:
	// 		return GL_COLOR_ATTACHMENT5;
	// 	case 6:
	// 		return GL_COLOR_ATTACHMENT6;
	// 	case 7:
	// 		return GL_COLOR_ATTACHMENT7;
	// 	default:
	// 		std::cerr << "Non supported GL color attachment index: " << ColorIndex << std::endl;
	// 	}
	//
	// 	return -1;
	// }

}
