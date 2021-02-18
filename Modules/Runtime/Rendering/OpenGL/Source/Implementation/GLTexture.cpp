#include "GLTexture.h"
#include "Bitmap.h"
#include "Core/Globals.h"
#include <iostream>

namespace Ry
{

	GLTexture::GLTexture():
	GLTexture(GL_TEXTURE_2D, GL_RGBA)
	{
		// Setup texture 2D parameters
		glBindTexture(GL_TEXTURE_2D, Handle);
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLTexture::GLTexture(GLenum Target, GLenum InternalFormat)
	{
		this->Target = Target;
		this->InternalFormat = InternalFormat;
		this->Handle = 0;
		
		glGenTextures(1, &Handle);
		Ry::Log->Log("OpenGL texture generated: " + Ry::to_string(Handle));
	}

	GLTexture::~GLTexture()
	{
		glDeleteTextures(1, &Handle);
	}

	void GLTexture::Bind(uint32 Unit)
	{
		glActiveTexture(GL_TEXTURE0 + Unit);
		glBindTexture(Target, Handle);		
		glActiveTexture(GL_TEXTURE0); // Switch back to default texture unit
	}

	void GLTexture::Data(void* Data, PixelFormat Format, PixelStorage Storage, uint32 Width, uint32 Height)
	{
		int32 GLFormat = PixelFormatToGL(Format);
		int32 GLStorage = PixelStorageToGL(Storage);
		
		glBindTexture(Target, Handle);
		{
			glTexImage2D(Target, 0, InternalFormat, Width, Height, 0, GLFormat, GLStorage, Data);
		}
		glBindTexture(Target, 0);
	}

	int32 GLTexture::GetSamples() const
	{
		return 1;
	}

	GLuint GLTexture::GetHandle()
	{
		return Handle;
	}

	GLenum GLTexture::GetTarget()
	{
		return Target;
	}

	GLenum GLTexture::GetInternalFormat()
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
	
}