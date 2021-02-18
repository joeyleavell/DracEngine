#pragma once

#include "Asset.h"
#include "Material.h"
#include "RenderAssetGen.h"

namespace Ry
{

	class RENDERASSET_MODULE MaterialAsset : public Asset
	{
	public:

		MaterialAsset(Material* Mat)
		{
			this->RuntimeMaterial = Mat;
		}
		
		virtual void UnloadAsset()
		{
			delete RuntimeMaterial;
		}

		Material* GetRuntimeMaterial()
		{
			return RuntimeMaterial;
		}

	private:
		Material* RuntimeMaterial;
	};
	
}
