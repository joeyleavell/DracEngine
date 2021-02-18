#include "Core/Core.h"
#include "Factory/TrueTypeFontFactory.h"
#include "VectorFontAsset.h"
#include "File/File.h"
#include <iostream>

#include "stb_truetype.h"

constexpr int64 MAX_FONT_SIZE = 50 * 1024 * 1024;

namespace Ry
{
	void TrueTypeFontFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		TTFFontData* LoadedData = new TTFFontData;

		// Allocate space for the font
		uint8* FontBuffer = new uint8[MAX_FONT_SIZE];
		memset(FontBuffer, 0, MAX_FONT_SIZE);
		
		// Load binary file
		uint64 LoadedBytes = Ry::File::LoadFileBytes(Reference.GetAbsolute(), FontBuffer, MAX_FONT_SIZE);

		// Create a new buffer with just enough space
		LoadedData->StoredFont = new uint8[LoadedBytes];
		MemCpy(LoadedData->StoredFont, LoadedBytes, FontBuffer, LoadedBytes);

		// Clear unneeded memory
		delete[] FontBuffer;

		// Load font info
		LoadedData->FontInfo = new stbtt_fontinfo;
		stbtt_InitFont(LoadedData->FontInfo, LoadedData->StoredFont, 0);
		
		AssetDst.push_back(new VectorFontAsset(LoadedData));
	}
}
