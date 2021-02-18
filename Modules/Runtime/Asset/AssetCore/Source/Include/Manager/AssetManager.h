#pragma once

#include "Manager/IAssetManager.h"
#include "Core/String.h"
#include "Data/Map.h"

namespace Ry
{
	class Asset;
	class AssetFactory;
	
	class ASSETCORE_MODULE AssetManager : public IAssetManager
	{
	public:

		AssetManager() = default;
		virtual ~AssetManager() = default;

		virtual void UnloadAll();
		virtual void RegisterFactory(const Ry::String& AssetType, AssetFactory* Factory);
		virtual void UnregisterFactory(const Ry::String& AssetType);
		virtual bool UnloadAsset(const Ry::AssetRef& Reference);

		virtual Asset* LoadAsset(const Ry::AssetRef& Reference, const Ry::String& AssetType);
		virtual Asset* GetOrLoadAsset(const Ry::AssetRef& Reference, const Ry::String& AssetType);
		virtual Asset* GetAsset(const Ry::AssetRef& Reference);


	private:

		Ry::Map<Ry::AssetRef, Ry::Asset*> LoadedAssets; // Asset reference to asset
		Ry::Map<Ry::String, Ry::AssetFactory*> Factories; // Asset type to factory
	};
}

