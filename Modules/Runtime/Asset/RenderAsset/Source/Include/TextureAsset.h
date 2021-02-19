#pragma once

#include "Asset.h"

namespace Ry
{
	
	class Bitmap;
	class Texture2;
	
	class TextureAsset : public Asset
	{
	public:

		TextureAsset(Bitmap* Resource);

		void UnloadAsset() override;
		Texture2* CreateRuntimeTexture();
		
	private:
		Ry::Bitmap* Resource;
		Ry::ArrayList<Texture2*> RuntimeResources;
		
	};
	
}
