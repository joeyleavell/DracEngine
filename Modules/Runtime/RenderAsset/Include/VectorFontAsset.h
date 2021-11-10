#pragma once

#include "Asset.h"
#include "Data/Map.h"
#include "RenderAssetGen.h"

struct stbtt_fontinfo;

namespace Ry
{
	class BitmapFont;

	struct TTFFontData
	{
		stbtt_fontinfo* FontInfo;
		uint8* StoredFont;
	};
	
	class RENDERASSET_MODULE VectorFontAsset : public Asset
	{
	public:

		VectorFontAsset();
		VectorFontAsset(TTFFontData* FontData);
		virtual ~VectorFontAsset();

		void UnloadAsset() override;

		/**
		 * Gets a generated bitmap font.
		 *
		 * @return BitmapFont The generated font, or null if that size has not yet been generated.
		 */
		Ry::BitmapFont* GetBitmapFont(int32 Size);
		Ry::BitmapFont* GenerateBitmapFont(int32 Size);
		Ry::BitmapFont* GetOrGenBitmapFont(int32 Size);

	private:

		Map<int32, Ry::BitmapFont*> GeneratedFonts;

		TTFFontData* TTFData;
	};
}
