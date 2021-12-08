#include "Factory/TextureFactory.h"
#include "TextureAsset.h"
#include "Bitmap.h"
#include <iostream>
#include "Core/Globals.h"
#include "stb_image.h"
#include "File/File.h"

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

	void TextureFactory::ImportAssets(const Ry::String& Path, Ry::ArrayList<NewAsset*>& NewAssets)
	{
		Ry::String ObjectName = Filesystem::path(*Path).stem().string().c_str();

		int32 Width, Height, Channels;
		uint8* PixelData = stbi_load(*Path, &Width, &Height, &Channels, 0);

		if (!PixelData)
		{
			std::cerr << "Failed to load image: " << *Path << std::endl;
		}
		else
		{
			TextureAsset2* Result = NewObject<TextureAsset2>(ObjectName);
			Result->Width = Width;
			Result->Height = Height;

			for(int32 PixelX = 0; PixelX < Width; PixelX++)
			{
				for (int32 PixelY = 0; PixelY < Height; PixelY++)
				{
					// Flip on the Y axis
					int32 PixelIndex = (Height - PixelY - 1) * Width + PixelX;

					uint8 Red = 0;
					uint8 Green = 0;
					uint8 Blue = 0;
					uint8 Alpha = 255;

					if (Channels >= 4)
					{
						Alpha = PixelData[PixelIndex * Channels + 3];
					}

					if (Channels >= 3)
					{
						Blue = PixelData[PixelIndex * Channels + 2];
					}

					if (Channels >= 2)
					{
						Green = PixelData[PixelIndex * Channels + 1];
					}

					if (Channels >= 1)
					{
						Red = PixelData[PixelIndex * Channels + 0];
					}

					// Insert pixel data
					Result->TextureData.Add(Red);
					Result->TextureData.Add(Green);
					Result->TextureData.Add(Blue);
					Result->TextureData.Add(Alpha);
				}
			}

			// Insert the result asset
			NewAssets.Add(Result);
		}
	}
}
