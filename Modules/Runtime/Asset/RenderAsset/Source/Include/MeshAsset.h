#pragma once

#include "Asset.h"
#include "RenderAssetGen.h"

namespace Ry
{
	class Shader;
	class MeshData;
	class Mesh2;

	class RENDERASSET_MODULE MeshAsset : public Asset
	{
	public:

		MeshAsset(Ry::MeshData* MeshData);
		
		void UnloadAsset() override;

		Mesh2* CreateRuntimeMesh() const;

	private:
		Ry::MeshData* Data;
		mutable Ry::ArrayList<Mesh2*> RuntimeMeshes;
		
	};
}
