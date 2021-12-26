#include "GLTexture.h"
#include "Core/Globals.h"
#include "Bitmap.h"

namespace Ry
{
	
	GLTexture::GLTexture(TextureUsage InUsage, TextureFiltering Filter):
	Texture(InUsage, Filter)
	{
		// Todo: support different types of textures

		this->Target = GL_TEXTURE_2D;
		this->InternalFormat = GL_SRGB8_ALPHA8;
		this->Handle = 0;

		glGenTextures(1, &Handle);

		GLint GLFilter = GL_LINEAR;
		if (Filter == TextureFiltering::Nearest)
			GLFilter = GL_NEAREST;
		
		// Setup texture 2D parameters
		glBindTexture(GL_TEXTURE_2D, Handle);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLFilter);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLTexture::Data(uint8* Data, uint32 Width, uint32 Height, PixelFormat Format)
	{
		Texture::Data(Data, Width, Height, Format);

		int32 GLFormat = PixelFormatToGLFormat(Format);
		int32 GLStorage = PixelFormatToGLStorage(Format);

		glBindTexture(Target, Handle);
		glTexImage2D(Target, 0, InternalFormat, Width, Height, 0, GLFormat, GLStorage, Data);
		glBindTexture(Target, 0);

	}

	void GLTexture::DeleteTexture()
	{
		glDeleteTextures(1, &Handle);
	}

	GLuint GLTexture::GetHandle() const
	{
		return Handle;
	}

	GLenum GLTexture::GetTarget() const
	{
		return Target;
	}

	GLenum GLTexture::GetInternalFormat() const
	{
		return InternalFormat;
	}

	int32 GLTexture::PixelFormatToGLFormat(PixelFormat Format)
	{
		int32 GLFormat = GL_RGB;

		switch (Format)
		{
		case PixelFormat::R8G8B8A8:
			GLFormat = GL_RGBA;
			break;
		case PixelFormat::R8G8B8:
			GLFormat = GL_RGB;
			break;
		default:
			Ry::Log->LogError("NON SUPPORTED FORMAT RETURNING GL_RGB " + Ry::to_string((int32)Format));
		}

		return GLFormat;
	}

	int32 GLTexture::PixelFormatToGLStorage(PixelFormat Format)
	{
		int32 GLStorage = GL_UNSIGNED_BYTE;

		switch (Format)
		{
		case PixelFormat::R8G8B8A8:
		case PixelFormat::R8G8B8:
			GLStorage = GL_UNSIGNED_BYTE;
			break;
		default:
			Ry::Log->LogError("Non supported GL format for textures: " + Ry::to_string((int32)Format));
		}

		return GLStorage;
	}
	
}
