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
		Ry::Log->Log("OpenGL texture generated: " + Ry::to_string(Handle));

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

	void GLTexture::Data(const Bitmap* Bitmap)
	{
		Texture::Data(Bitmap);
		//this->InternalFormat = PixelFormatToGL(Bitmap->GetPixelBuffer()->GetPixelFormat());
	
		uint32 Width = Bitmap->GetWidth();
		uint32 Height = Bitmap->GetHeight();
		uint8* RawData = Bitmap->GetData<uint8>();

		int32 GLFormat = PixelFormatToGL(Bitmap->GetPixelBuffer()->GetPixelFormat());
		int32 GLStorage = PixelStorageToGL(Bitmap->GetPixelBuffer()->GetPixelStorage());

		glBindTexture(Target, Handle);
		glTexImage2D(Target, 0, InternalFormat, Width, Height, 0, GLFormat, GLStorage, RawData);
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

	int32 GLTexture::PixelFormatToGL(PixelFormat Format)
	{
		int32 GLFormat = GL_RGB;

		switch (Format)
		{
		case PixelFormat::NONE:
		case PixelFormat::RGB:
			GLFormat = GL_RGB;
			break;
		case PixelFormat::RGBA:
			GLFormat = GL_RGBA;
			break;
		case PixelFormat::GRAYSCALE:
			GLFormat = GL_RED;
			break;
		case PixelFormat::DEPTH:
			GLFormat = GL_DEPTH_COMPONENT;
			break;
		default:
			Ry::Log->LogError("NON SUPPORTED FORMAT RETURNING GL_RGB " + Ry::to_string((int32)Format));
		}

		return GLFormat;
	}

	int32 GLTexture::PixelStorageToGL(PixelStorage Storage)
	{
		int32 GLStorage = GL_UNSIGNED_BYTE;

		switch (Storage)
		{
		case PixelStorage::NONE:
		case PixelStorage::RED8:
		case PixelStorage::THREE_BYTE_RGB:
		case PixelStorage::FOUR_BYTE_RGBA:
			GLStorage = GL_UNSIGNED_BYTE;
			break;
		case PixelStorage::FLOAT:
			GLStorage = GL_FLOAT;
			break;
		default:
			Ry::Log->LogError("NON SUPPORTED FORMAT RETURNING GL_RGB " + Ry::to_string((int32)Storage));
		}

		return GLStorage;
	}

	// Todo: move this concept to gl resources/command buffer
// void GLTexture::Bind(uint32 Unit)
// {
// 	glActiveTexture(GL_TEXTURE0 + Unit);
// 	glBindTexture(Target, Handle);		
// 	glActiveTexture(GL_TEXTURE0); // Switch back to default texture unit
// }
	
}
