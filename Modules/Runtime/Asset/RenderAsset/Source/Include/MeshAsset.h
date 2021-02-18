#pragma once

#include "Asset.h"
#include "RenderAssetGen.h"

namespace Ry
{
	class Shader;
	class MeshData;
	class Mesh;

	class RENDERASSET_MODULE MeshAsset : public Asset
	{
	public:

		MeshAsset(Ry::MeshData* MeshData);
		
		void UnloadAsset() override;

		Ry::SharedPtr<Mesh> CreateRuntimeMesh(Shader* Shader) const;

	private:
		Ry::MeshData* Data;
	};
}
