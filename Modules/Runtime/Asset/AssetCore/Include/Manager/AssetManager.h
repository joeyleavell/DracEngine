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

		void UnloadAll() override;
		void RegisterFactory(const Ry::String& AssetType, AssetFactory* Factory) override;
		void UnregisterFactory(const Ry::String& AssetType) override;
		bool UnloadAsset(const Ry::AssetRef& Reference) override;

	protected:
		
		Asset* LoadAssetInternal(const Ry::AssetRef& Reference, const Ry::String& AssetType) override;
		Asset* GetOrLoadAssetInternal(const Ry::AssetRef& Reference, const Ry::String& AssetType) override;
		Asset* GetAssetInternal(const Ry::AssetRef& Reference) override;


	private:

		Ry::Map<Ry::AssetRef, Ry::Asset*> LoadedAssets; // Asset reference to asset
		Ry::Map<Ry::String, Ry::AssetFactory*> Factories; // Asset type to factory
	};
}

