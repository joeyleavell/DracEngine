#pragma once

#include "Factory/AssetFactory.h"
#include "RenderAssetGen.h"

namespace Ry
{
	class RENDERASSET_MODULE TrueTypeFontFactory : public AssetFactory
	{
	public:

		virtual void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;
	};
}
