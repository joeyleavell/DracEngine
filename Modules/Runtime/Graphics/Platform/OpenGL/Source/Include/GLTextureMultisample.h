#pragma once

#include "Core/Core.h"
#include "GLTexture.h"
#include "glew.h"

namespace Ry
{

	class OPENGL_MODULE GLTextureMultisample : public GLTexture
	{
	public:
		GLTextureMultisample(int32 Width, int32 Height, int32 Samples, GLenum InternalFormat);

		int32 GetSamples() const override;
		
		/**
		 * You cannot update a multisample texture with client data so this function has no implementation
		 */
		void Data(void* Data, PixelFormat Format, PixelStorage Storage, uint32 Width, uint32 Height) override
		{
			
		}

	private:

		int32 Samples;
	};
	
}
