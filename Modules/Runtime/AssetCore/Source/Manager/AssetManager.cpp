#include "Manager/AssetManager.h"
#include "Factory/AssetFactory.h"
#include "Asset.h"

#include <iostream>

namespace Ry
{
	void AssetManager::UnloadAll()
	{
		KeyIterator<Ry::AssetRef, Asset*> AssetKeyItr = LoadedAssets.CreateKeyIterator();
		KeyIterator<Ry::String, AssetFactory*> FactoryKeyItr = Factories.CreateKeyIterator();

		// Unload all assets
		while (AssetKeyItr)
		{
			Ry::AssetRef* Ref = AssetKeyItr.Key();
			
			(*AssetKeyItr.Value())->UnloadAsset();
			delete *LoadedAssets.get(*Ref);

			++AssetKeyItr;
		}

		// Unload all factories
		while (FactoryKeyItr)
		{
			delete *FactoryKeyItr.Value();			
			++FactoryKeyItr;
		}

		LoadedAssets.Clear();
		Factories.Clear();
	}

	void AssetManager::RegisterFactory(const Ry::String& AssetType, AssetFactory* Factory)
	{
		Factories.insert(AssetType.to_lower(), Factory);
	}

	void AssetManager::UnregisterFactory(const Ry::String& AssetType)
	{
		Ry::String Canon = AssetType.to_lower();
		if (Factories.contains(Canon))
		{
			AssetFactory** Factory = Factories.get(Canon);

			if (Factory)
			{
				Factories.remove(Canon);
				delete* Factory;
			}
			else
			{
				// TODO: Factory was null for some reason, handle
			}
		}
		else
		{
			// TODO: Factories did not contain factory, handle
		}
	}

	Asset* AssetManager::LoadAssetInternal(const Ry::AssetRef& Reference, const Ry::String& AssetType)
	{
		Ry::String Canon = AssetType.to_lower();
		if (Factories.contains(Canon))
		{
			AssetFactory** Factory = Factories.get(Canon);

			if (Factory)
			{
				std::vector<Asset*> Results;
				(*Factory)->LoadAssets(Reference, Results);

				if(Results.size() > 0)
				{
					// Put the asset in the loaded assets directory
					LoadedAssets.insert(Reference, Results.at(0));

					return Results.at(0);
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				std::cerr << "The factory registered with asset type " << *AssetType << " was null" << std::endl;
			}
		}
		else
		{
			std::cerr << "There is no factory registered with asset type " << *AssetType << std::endl;
		}

		return nullptr;
	}

	Asset* AssetManager::GetOrLoadAssetInternal(const Ry::AssetRef& Reference, const Ry::String& AssetType)
	{
		Asset* Current = GetAssetInternal(Reference);

		if (Current)
		{
			return Current;
		}
		else
		{
			return LoadAssetInternal(Reference, AssetType);
		}
	}

	Asset* AssetManager::GetAssetInternal(const Ry::AssetRef& Reference)
	{
		if (LoadedAssets.contains(Reference))
		{
			Asset** LoadedAsset = LoadedAssets.get(Reference);

			if (LoadedAsset)
			{
				return *LoadedAsset;
			}
			else
			{
				std::cerr << "Asset " << *Reference << " was null" << std::endl;
			}
		}

		return nullptr;
	}

	bool AssetManager::UnloadAsset(const Ry::AssetRef& Reference)
	{
		if (LoadedAssets.contains(Reference))
		{
			Asset** LoadedAsset = LoadedAssets.get(Reference);

			if (LoadedAsset)
			{
				(*LoadedAsset)->UnloadAsset();
				LoadedAssets.remove(Reference);
			}
			else
			{
				std::cerr << "Asset " << *Reference << " was null" << std::endl;
			}
		}

		return false;
	}
}