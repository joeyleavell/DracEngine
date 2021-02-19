#pragma once

#include "Core/Core.h"
#include "RenderingGen.h"

namespace Ry
{
	class Bitmap;
	enum class PixelFormat;
	enum class PixelStorage;

	enum class TextureUsage
	{
		STATIC, DYNAMIC	
	};
	
	class RENDERING_MODULE Texture2
	{
	
	public:
	
		Texture2(TextureUsage InUsage);
		virtual ~Texture2() = default;

		TextureUsage GetUsage() const;
		
		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/
		virtual void Data(const Bitmap* Bitmap) = 0;
		virtual void DeleteTexture() = 0;


	private:
		
		TextureUsage Usage;
	
	};
}
