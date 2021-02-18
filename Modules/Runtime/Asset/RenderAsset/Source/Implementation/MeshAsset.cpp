#include "MeshAsset.h"
#include "Mesh.h"

namespace Ry
{
	MeshAsset::MeshAsset(Ry::MeshData* MeshData)
	{
		this->Data = MeshData;
	}

	void MeshAsset::UnloadAsset()
	{
		
	}

	Ry::SharedPtr<Mesh> MeshAsset::CreateRuntimeMesh(Shader* Shader) const
	{
		SharedPtr<Mesh> NewMesh = Ry::MakeShared(new Mesh(Data, BufferHint::STATIC));
		NewMesh->GetMeshData()->SetShaderAll(Shader);

		return NewMesh;
	}
	
}