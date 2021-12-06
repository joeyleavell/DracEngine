#pragma once

#include "Core/String.h"
#include "Asset.h"
#include "File/AssetRef.h"
#include <vector>

namespace Ry
{
	
	class ASSETCORE_MODULE AssetFactory
	{
	public:

		AssetFactory() = default;
		virtual ~AssetFactory() = default;

		/**
		 * Called when an asset needs to be loaded. The factory should already know what type of asset it is loading.
		 * @param Reference The path to the asset that needs to be loaded.
		 */
		virtual void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) = 0;

	};
}
