#pragma once

#include "Core/String.h"
#include "Asset.h"

namespace Ry
{
	class AssetFactory;

	extern ASSETCORE_MODULE Ry::String ASSET_TYPE_TEXT;
	extern ASSETCORE_MODULE Ry::String ASSET_TYPE_TEXTURE;
	
	class ASSETCORE_MODULE IAssetManager
	{
	public:

		IAssetManager() = default;
		virtual ~IAssetManager() = default;

		/**
		 * Unloads all loaded assets and asset factories.
		 */
		virtual void UnloadAll() = 0;

		virtual void RegisterFactory(const Ry::String& AssetType, AssetFactory* Factory) = 0;
		virtual void UnregisterFactory(const Ry::String& AssetType) = 0;

		virtual Asset* LoadAsset(const Ry::AssetRef& Reference, const Ry::String& AssetType) = 0;

		/**
		* Retreieves the asset by the specified path. If the asset has not already been loaded, the manager will automatically load the asset.
		*
		* @param Reference The path of the asset to load, in virtual file space.
		* @return The loaded asset, or null if the asset could not be loaded
		*/
		virtual Asset* GetOrLoadAsset(const Ry::AssetRef& Reference, const Ry::String& AssetType) = 0;

		/**
		* Retreieves the asset by the specified path only if it is already loaded.
		*
		* @param Reference The path of the asset to load, in virtual file space.
		* @return The loaded asset, or null if that asset has not been loaded yet.
		*/
		virtual Asset* GetAsset(const Ry::AssetRef& Reference) = 0;

		/**
		 * Unloads the specified asset from memory.
		 */
		virtual bool UnloadAsset(const Ry::AssetRef& Reference) = 0;

	};
	
	extern ASSETCORE_MODULE IAssetManager* AssetMan;
	
}
