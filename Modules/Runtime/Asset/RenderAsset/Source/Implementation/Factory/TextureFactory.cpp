#include "Factory/TextureFactory.h"
#include "TextureAsset.h"
#include "Bitmap.h"
#include <iostream>
#include "stb_image.h"

namespace Ry
{
	
	void TextureFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		int32 Width, Height, Channels;
		uint8* PixelData = stbi_load(*Reference.GetAbsolute(), &Width, &Height, &Channels, 0);

		if (!PixelData)
		{
			std::cerr << "Failed to load image: " << *Reference.GetVirtual() << std::endl;
		}
		else
		{
			Bitmap* Result = nullptr;

			if (Channels == 3)
			{
				Result = new Bitmap(Width, Height, PixelStorage::THREE_BYTE_RGB);
			}
			else if (Channels == 4)
			{
				Result = new Bitmap(Width, Height, PixelStorage::FOUR_BYTE_RGBA);
			}

			Result->SetData(PixelData);

			// Flip data
			Result->FlipY();

			AssetDst.push_back(new TextureAsset(Result));
		}
	}

}
