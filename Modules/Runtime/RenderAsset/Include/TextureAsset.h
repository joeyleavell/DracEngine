#pragma once

#include "Asset.h"
#include "TextureAsset.gen.h"

namespace Ry
{
	
	class Bitmap;
	class Texture;

	struct RENDERASSET_MODULE TextureAsset2 : public Asset
	{
		RefField()
		uint32 Width;

		RefField()
		uint32 Height;

		RefField()
		Ry::ArrayList<uint8> TextureData;

	} RefClass();
	
	class RENDERASSET_MODULE TextureAsset : public Asset
	{
	public:

		TextureAsset(Bitmap* Resource);

		void UnloadAsset() override;
		Texture* CreateRuntimeTexture();

		Ry::Bitmap* GetResource();
		
	private:
		Ry::Bitmap* Resource;
		Ry::ArrayList<Texture*> RuntimeResources;
		
	};
	
}
