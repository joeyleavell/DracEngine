#pragma once

#include "Factory/AssetFactory.h"
#include "RenderAssetGen.h"

namespace Ry
{


	class RENDERASSET_MODULE TextureFactory : public AssetFactory
	{
	public:
		TextureFactory() = default;
		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;
	};
	
}
