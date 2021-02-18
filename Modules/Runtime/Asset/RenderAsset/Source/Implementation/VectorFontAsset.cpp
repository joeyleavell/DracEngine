#include "VectorFontAsset.h"
#include "Font.h"
#include <iostream>

#include "stb_truetype.h"

constexpr uint32 START_CODEPOINT = 32;
constexpr uint32 END_CODEPOINT = 126;

namespace Ry
{

	VectorFontAsset::VectorFontAsset():
	TTFData(nullptr)
	{
		
	}
	
	VectorFontAsset::VectorFontAsset(TTFFontData* FontData)
	{
		this->TTFData = FontData;
	}
	
	VectorFontAsset ::~VectorFontAsset()
	{
		
	}
	
	void VectorFontAsset::UnloadAsset()
	{
		if(TTFData)
		{
			// Free all cached fonts
			KeyIterator<int32, Ry::BitmapFont*> KeyItr = GeneratedFonts.CreateKeyIterator();

			while(KeyItr)
			{
				delete KeyItr.Value();				
				++KeyItr;
			}
			
			delete TTFData->StoredFont;
			delete TTFData->FontInfo;
			delete TTFData;
		}
	}

	Ry::BitmapFont* VectorFontAsset::GetBitmapFont(int32 Size)
	{
		if(GeneratedFonts.contains(Size))
		{
			return *GeneratedFonts.get(Size);
		}
		else
		{
			return nullptr;
		}
	}

	Ry::BitmapFont* VectorFontAsset::GenerateBitmapFont(int32 Size)
	{
		BitmapFont* Result = new BitmapFont();
		
		float Scale = (float) stbtt_ScaleForMappingEmToPixels(TTFData->FontInfo, static_cast<float>(Size));

		int32 Ascent;
		int32 Descent;
		int32 LineGap;
		int32 X0, Y0, X1, Y1;
		int32 RasterWidth, RasterHeight;
		int32 AdvanceWidth, LeftSideBearing;

		stbtt_GetFontVMetrics(TTFData->FontInfo, &Ascent, &Descent, &LineGap);

		// Set font vertical parameters
		Result->SetAscent((int32) (Ascent * Scale));
		Result->SetDescent((int32)(Descent * Scale));
		Result->SetLineGap((int32)(LineGap * Scale));
		Result->SetCodepointRange(START_CODEPOINT, END_CODEPOINT);

		for (uint32 Codepoint = START_CODEPOINT; Codepoint <= END_CODEPOINT; Codepoint++)
		{
			auto* NewGlyph = new BitmapGlyph;
			
			stbtt_GetCodepointHMetrics(TTFData->FontInfo, Codepoint, &AdvanceWidth, &LeftSideBearing);

			stbtt_GetCodepointBitmapBox(TTFData->FontInfo, Codepoint, Scale, Scale, &X0, &Y0, &X1, &Y1);
			RasterWidth = X1 - X0 + 1;
			RasterHeight = Y1 - Y0 + 1;

			// Check for zero area codepoints
			if (X1 == X0 && Y1 == Y0)
			{
				RasterWidth = 0;
				RasterHeight = 0;
			}

			auto* Raster = new uint8[(uint64) RasterWidth * RasterHeight];

			stbtt_MakeCodepointBitmap(TTFData->FontInfo, Raster, RasterWidth, RasterHeight, RasterWidth, Scale, Scale, Codepoint);

			// Set the glyph specific parameters
			NewGlyph->OriginX = X0;
			NewGlyph->OriginY = Y0;
			NewGlyph->LeftSideBearing = (int32) (LeftSideBearing * Scale);
			NewGlyph->AdvanceWidth = (int32) (AdvanceWidth * Scale);
			NewGlyph->Raster = new Bitmap(RasterWidth, RasterHeight, PixelStorage::RED8);
			NewGlyph->Raster->SetData(Raster);

			/**
			 * Build the kerning table for this entry
			 */
			for (uint32 ToKern = START_CODEPOINT; ToKern <= END_CODEPOINT; ToKern++)
			{
				int32 Kerning = stbtt_GetCodepointKernAdvance(TTFData->FontInfo, Codepoint, ToKern);
				NewGlyph->KerningTable.insert(ToKern, Kerning);
			}

			Result->AddGlyph(Codepoint, NewGlyph);
		}

		// Generate the rendering resources for the font.
		Result->CreateRenderingResources();

		// Cache the generated font
		GeneratedFonts.insert(Size, Result);

		return Result;
	}
	
}
