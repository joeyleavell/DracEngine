#include "MeshAsset.h"
#include "Mesh2.h"

namespace Ry
{
	MeshAsset::MeshAsset(Ry::MeshData* MeshData)
	{
		this->Data = MeshData;
	}

	void MeshAsset::UnloadAsset()
	{
		// Destroy all runtime meshes

		for(Mesh2* RuntimeMesh : RuntimeMeshes)
		{
			RuntimeMesh->DeleteMesh();
			delete RuntimeMesh;
		}
		
	}

	Mesh2* MeshAsset::CreateRuntimeMesh() const
	{
		Mesh2* NewMesh = new Mesh2(Data);

		RuntimeMeshes.Add(NewMesh);

		return NewMesh;
	}
	
}
