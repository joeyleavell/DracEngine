#pragma once

#include "Core/Core.h"
#include "Texture.gen.h"

namespace Ry
{
	enum class TextureUsage
	{
		STATIC, DYNAMIC	
	};

	enum class TextureFiltering
	{
		Nearest, Linear
	};

	enum class PixelFormat
	{
		R8G8B8A8, R8G8B8
	};
	
	class ARI_MODULE Texture
	{
	
	public:
	
		Texture(TextureUsage InUsage, TextureFiltering Filter);
		virtual ~Texture() = default;

		TextureUsage GetUsage() const;
		TextureFiltering GetFilter() const;
		
		virtual void Data(uint8* Data, uint32 Width, uint32 Height, PixelFormat Format);
		virtual void DeleteTexture() = 0;

		int32 GetWidth() const;
		int32 GetHeight() const;

	private:

		uint32 Width;
		uint32 Height;
		TextureUsage Usage;
		TextureFiltering Filter;
	
	};
}
