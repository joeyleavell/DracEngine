#include "Texture.h"
#include "Bitmap.h"

namespace Ry
{

	Texture::Texture(TextureUsage InUsage, TextureFiltering Filter)
	{
		this->Usage = InUsage;
		this->Filter = Filter;
		this->CachedBitmap = nullptr;
	}
	
	TextureUsage Texture::GetUsage() const
	{
		return Usage;
	}

	TextureFiltering Texture::GetFilter() const
	{
		return Filter;
	}

	int32 Texture::GetWidth() const
	{
		return CachedBitmap->GetWidth();
	}

	int32 Texture::GetHeight() const
	{
		return CachedBitmap->GetHeight();
	}
	
}
