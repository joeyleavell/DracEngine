#pragma once

#include "Core/Core.h"
#include "Data/Map.h"
#include "RenderingGen.h"

namespace Ry
{
	class ShapeBatch;
	class TextureBatch;
	class String;
	class Texture;
	class Bitmap;

	constexpr uint32 FONT_BITMAP_SIZE = 1024;

	struct RENDERING_MODULE BitmapGlyph
	{
		int32 AdvanceWidth;
		int32 LeftSideBearing;
		int32 OriginX;
		int32 OriginY;

		int32 SheetX;
		int32 SheetY;
		Bitmap* Raster;

		/**
		 * Maps a codepoint to a kerning amount
		 */
		Ry::Map<int32, int32> KerningTable;

		BitmapGlyph():
		AdvanceWidth(0),
		LeftSideBearing(0),
		OriginX(0),
		OriginY(0),
		SheetX(0),
		SheetY(0),
		Raster(nullptr)
		{
			
		}
	};

	class RENDERING_MODULE BitmapFont
	{
	public:
		BitmapFont();
		~BitmapFont();

		int32 GetAscent() const;
		int32 GetDescent() const;
		int32 GetLineGap() const;
		int32 GetMinCodepoint() const;
		int32 GetMaxCodepoint() const;
		BitmapGlyph* GetGlyph(int32 Codepoint) const;
		Ry::Texture* GetAtlasTexture() const;

		void SetAscent(int32 Ascent);
		void SetDescent(int32 Descent);
		void SetLineGap(int32 LineGap);
		void SetCodepointRange(int32 MinCodepoint, int32 MaxCodepoint);
		void AddGlyph(int32 Codepoint, BitmapGlyph* Glyph);
		void RemoveGlyph(int32 Codepoint);

		void CreateRenderingResources();
		void DeleteRenderingResources();

		float MeasureHeight(const Ry::String& String, float WrapWidth);
		float MeasureWidth(const Ry::String& String);

		void MeasureXOffsets(Ry::ArrayList<float>& OutOffsets, const Ry::String& String);

	private:

		Ry::OAHashMap<Ry::String, float> MeasuredWidth;
		Ry::OAHashMap<Ry::String, float> MeasuredHeight;

		int32 Ascent;
		int32 Descent;
		int32 LineGap;
		int32 MinCodepoint;
		int32 MaxCodepoint;
		Ry::Map<int32, Ry::BitmapGlyph*> RegisteredGlyphs;
		class Texture* AtlasTexture;
	};
	
}
