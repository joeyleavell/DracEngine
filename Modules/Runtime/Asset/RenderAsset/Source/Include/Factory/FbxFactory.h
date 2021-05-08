#pragma once

#include "Factory/AssetFactory.h"
#include <vector>
#include "RenderAssetGen.h"

namespace Ry
{
	struct Material;

	class RENDERASSET_MODULE FbxFactory : public AssetFactory
	{
		
		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;
		
	};

}
