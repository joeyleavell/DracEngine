#pragma once

#include "Texture.h"
#include "Core/Core.h"
#include "glew.h"
#include "OpenGLGen.h"

namespace Ry
{
	class OPENGL_MODULE GLTexture : public Texture
	{
	public:

		GLTexture(TextureUsage InUsage, TextureFiltering Filter);
		virtual ~GLTexture() = default;
		
		void Data(uint8* Data, uint32 Width, uint32 Height, PixelFormat Format) override;
		void DeleteTexture() override;

		GLuint GetHandle() const;
		GLenum GetTarget() const;
		GLenum GetInternalFormat() const;

	protected:

		int32 PixelFormatToGLFormat(PixelFormat Format);
		int32 PixelFormatToGLStorage(PixelFormat Storage);
		
		GLuint Handle;
		GLenum Target;
		GLenum InternalFormat;
	};
}
