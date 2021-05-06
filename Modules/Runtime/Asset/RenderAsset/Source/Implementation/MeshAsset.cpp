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
		// Destroy all runtime meshes

		for(Mesh* RuntimeMesh : RuntimeMeshes)
		{
			RuntimeMesh->DeleteMesh();
			delete RuntimeMesh;
		}
		
	}

	Mesh* MeshAsset::CreateRuntimeMesh() const
	{
		Mesh* NewMesh = new Mesh(Data);

		RuntimeMeshes.Add(NewMesh);

		return NewMesh;
	}
	
}
