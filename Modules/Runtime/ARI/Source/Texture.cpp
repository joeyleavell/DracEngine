#include "Texture.h"

namespace Ry
{

	Texture::Texture(TextureUsage InUsage, TextureFiltering Filter)
	{
		this->Usage = InUsage;
		this->Filter = Filter;
	}
	
	TextureUsage Texture::GetUsage() const
	{
		return Usage;
	}

	TextureFiltering Texture::GetFilter() const
	{
		return Filter;
	}

	void Texture::Data(uint8* Data, uint32 Width, uint32 Height, PixelFormat Format)
	{
		this->Width = Width;
		this->Height = Height;
	}

	int32 Texture::GetWidth() const
	{
		return Width;
	}

	int32 Texture::GetHeight() const
	{
		return Height;
	}
	
}
