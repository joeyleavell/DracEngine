#pragma once

#include "Asset.h"

namespace Ry
{
	
	class Bitmap;
	class Texture;
	
	class TextureAsset : public Asset
	{
	public:

		TextureAsset(Bitmap* Resource);

		void UnloadAsset() override;
		Texture* CreateRuntimeTexture();
		
	private:
		Ry::Bitmap* Resource;
	};
	
}
