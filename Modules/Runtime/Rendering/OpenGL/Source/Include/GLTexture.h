#pragma once

#include "Interface/Texture.h"
#include "Core/Core.h"
#include "glew.h"
#include "OpenGLGen.h"

namespace Ry
{
	class OPENGL_MODULE GLTexture : public Texture
	{
	public:

		GLTexture();
		GLTexture(GLenum Target, GLenum InternalFormat);
		~GLTexture();

		void Bind(uint32 Unit) override;
		void Data(void* Data, PixelFormat Format, PixelStorage Storage, uint32 Width, uint32 Height) override;
		int32 GetSamples() const override;

		GLuint GetHandle();
		GLenum GetTarget();
		GLenum GetInternalFormat();

	protected:

		int32 PixelFormatToGL(PixelFormat Format);
		int32 PixelStorageToGL(PixelStorage Storage);
		
		GLuint Handle;
		GLenum Target;
		GLenum InternalFormat;
	};
}
