#include "Factory/TextureFactory.h"
#include "TextureAsset.h"
#include "Bitmap.h"
#include <iostream>
#include "Core/Globals.h"
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

			uint8* UsedData = PixelData;

			if(Channels == 1)
			{
				// TODO: this is temp, create grayscale images in rendering API
				// make grayscale rgb triplett

				UsedData = new uint8[Width * Height * 3];
				for(int X = 0; X < Width; X++)
				{
					for(int Y = 0; Y < Height; Y++)
					{
						int32 Index = X + Y * Width;
						UsedData[Index * 3 + 0] = PixelData[Index];
						UsedData[Index * 3 + 1] = PixelData[Index];
						UsedData[Index * 3 + 2] = PixelData[Index];
					}
				}
				
				Result = new Bitmap(Width, Height, PixelStorage::THREE_BYTE_RGB);

				delete[] PixelData;
			}
			else if (Channels == 3)
			{
				Result = new Bitmap(Width, Height, PixelStorage::THREE_BYTE_RGB);
			}
			else if (Channels == 4)
			{
				Result = new Bitmap(Width, Height, PixelStorage::FOUR_BYTE_RGBA);
			}

			if(Result)
			{
				Result->SetData(UsedData);

				// Flip data
				Result->FlipY();

				AssetDst.push_back(new TextureAsset(Result));
			}
			else
			{
//				Ry::Log->LogErrorf("Unsupported ")
			}


		}
	}

}
