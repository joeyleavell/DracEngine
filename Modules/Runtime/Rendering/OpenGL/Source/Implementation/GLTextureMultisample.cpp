#pragma once

#include "GLTextureMultisample.h"
#include <iostream>

namespace Ry
{
	
	GLTextureMultisample::GLTextureMultisample(int32 Width, int32 Height, int32 Samples, GLenum InternalFormat):
	GLTexture(GL_TEXTURE_2D_MULTISAMPLE, InternalFormat)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, GetHandle());
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, InternalFormat, Width, Height, GL_TRUE);

		this->Samples = Samples;
	}

	int32 GLTextureMultisample::GetSamples() const
	{
		return Samples;
	}

}

