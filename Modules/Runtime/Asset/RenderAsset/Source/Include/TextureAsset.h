#pragma once

#include "Asset.h"
#include "RenderAssetGen.h"

namespace Ry
{
	
	class Bitmap;
	class Texture2;
	
	class RENDERASSET_MODULE TextureAsset : public Asset
	{
	public:

		TextureAsset(Bitmap* Resource);

		void UnloadAsset() override;
		Texture2* CreateRuntimeTexture();

		Ry::Bitmap* GetResource();
		
	private:
		Ry::Bitmap* Resource;
		Ry::ArrayList<Texture2*> RuntimeResources;
		
	};
	
}
