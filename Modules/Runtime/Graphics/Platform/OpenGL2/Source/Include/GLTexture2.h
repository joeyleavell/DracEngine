#pragma once

#include "Interface2/Texture2.h"
#include "Core/Core.h"
#include "glew.h"
#include "OpenGL2Gen.h"

namespace Ry
{
	class OPENGL2_MODULE GLTexture2 : public Texture2
	{
	public:

		GLTexture2(TextureUsage InUsage);
		virtual ~GLTexture2() = default;
		
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
