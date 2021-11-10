#pragma once

#include "Factory/AssetFactory.h"

namespace Ry
{

	class ObjMtlFactory : public AssetFactory
	{
	public:

		Ry::String BuildName(int32 BeginToken, int32 TokenCount, Ry::String** Tokens);

		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;

	private:
		
	};
	
}
