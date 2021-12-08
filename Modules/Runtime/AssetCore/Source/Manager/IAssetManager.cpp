#include "Manager/IAssetManager.h"

namespace Ry
{
	
	Ry::String ASSET_TYPE_TEXT = "text";
	Ry::String ASSET_TYPE_TEXTURE = "texture";
	
	IAssetManager* AssetMan = nullptr;

	void IAssetManager::ImportAssets(const Ry::AssetRef& ParentDirectory, const Ry::ArrayList<Ry::String> Paths)
	{
		for(const Ry::String& Path : Paths)
		{
			ImportAsset(ParentDirectory, Path);
		}
	}

}
