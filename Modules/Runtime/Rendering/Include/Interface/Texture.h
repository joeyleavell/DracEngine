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

	enum class TextureFiltering
	{
		Nearest, Linear
	};
	
	class RENDERING_MODULE Texture
	{
	
	public:
	
		Texture(TextureUsage InUsage, TextureFiltering Filter);
		virtual ~Texture() = default;

		TextureUsage GetUsage() const;
		TextureFiltering GetFilter() const;
		
		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/
		virtual void Data(const Bitmap* Bitmap) { this->CachedBitmap = Bitmap; }
		virtual void DeleteTexture() = 0;

		int32 GetWidth() const;
		int32 GetHeight() const;


	private:

		const Bitmap* CachedBitmap;		
		TextureUsage Usage;
		TextureFiltering Filter;
	
	};
}
