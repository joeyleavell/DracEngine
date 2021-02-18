#pragma once

#include "Factory/AssetFactory.h"

namespace Ry
{
	class ASSETCORE_MODULE TextFileFactory : public AssetFactory
	{
		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;
	};
}
