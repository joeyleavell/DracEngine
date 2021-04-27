#include "GLTexture2.h"
#include "Core/Globals.h"
#include "Bitmap.h"

namespace Ry
{
	
	GLTexture2::GLTexture2(TextureUsage InUsage):
	Texture2(InUsage)
	{
		// Todo: support different types of textures

		this->Target = GL_TEXTURE_2D;
		this->InternalFormat = GL_RGBA;
		this->Handle = 0;

		glGenTextures(1, &Handle);
		Ry::Log->Log("OpenGL texture generated: " + Ry::to_string(Handle));
		
		// Setup texture 2D parameters
		glBindTexture(GL_TEXTURE_2D, Handle);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLTexture2::Data(const Bitmap* Bitmap)
	{
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

	void GLTexture2::DeleteTexture()
	{
		glDeleteTextures(1, &Handle);
	}

	GLuint GLTexture2::GetHandle() const
	{
		return Handle;
	}

	GLenum GLTexture2::GetTarget() const
	{
		return Target;
	}

	GLenum GLTexture2::GetInternalFormat() const
	{
		return InternalFormat;
	}

	int32 GLTexture2::PixelFormatToGL(PixelFormat Format)
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

	int32 GLTexture2::PixelStorageToGL(PixelStorage Storage)
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
