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

		GLTexture(TextureUsage InUsage);
		virtual ~GLTexture() = default;
		
		void Data(const Bitmap* Bitmap) override;
		void DeleteTexture() override;

		GLuint GetHandle() const;
		GLenum GetTarget() const;
		GLenum GetInternalFormat() const;

	protected:

		int32 PixelFormatToGL(PixelFormat Format);
		int32 PixelStorageToGL(PixelStorage Storage);
		
		GLuint Handle;
		GLenum Target;
		GLenum InternalFormat;
	};
}
