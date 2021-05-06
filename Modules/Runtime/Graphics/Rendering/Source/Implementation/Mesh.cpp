#include "Mesh.h"
#include "MeshData.h"
#include "Material.h"
#include "Interface/Shader.h"
#include "Interface/VertexArray.h"
#include "Core/Globals.h"
#include "File/File.h"
#include "Physics.h"
#include "Interface/RenderAPI.h"

namespace Ry
{
		
	Mesh::Mesh(const VertexFormat& Format)
	{
		this->Hint = Hint;
		this->VertArray = RendAPI->CreateVertexArray(Format);
		this->Data = new MeshData;
		this->Data->SetVertFormat(Format);
		this->StoredBoundingBox = nullptr;
	}

	Mesh::Mesh(MeshData* Data)
	{
		this->Data = Data;
		this->VertArray = RendAPI->CreateVertexArray(Data->GetVertFormat());
		this->StoredBoundingBox = nullptr;

		Update();
	}
	
	Mesh::~Mesh()
	{
		
	}
	
	void Mesh::DeleteMesh()
	{
		VertArray->DeleteArray();
		delete VertArray;
	}
	
	void Mesh::Update()
	{
		VertArray->PushVertexData(Data->GetVertData(), Data->GetVertexCount());
		VertArray->PushIndexData(Data->GetIndexData(), Data->GetIndexCount());
	}
	
	// void Mesh::Render(Primitive prim)
	// {
	// 	Mesh::Render(prim, Ry::IDENTITY_4, Ry::IDENTITY_4, nullptr);
	// }
	//
	// void Mesh::Render(Primitive prim, const Ry::Matrix4& Model, const Ry::Matrix4& ViewProjection, const SceneLighting* Lighting)
	// {
	// 	KeyIterator<int32, MeshSection> KeyItr = Data->Sections.CreateKeyIterator();
	//
	// 	while (KeyItr)
	// 	{
	// 		MeshSection* Section = Data->Sections.get(**KeyItr);
	// 		Material* Material = Section->Mat;
	// 		Shader* Shader = Section->Shad;
	// 		int32 StartIndex = Section->StartIndex;
	// 		int32 Count = Section->Count;
	//
	// 		Shader->bind();
	// 		{
	// 			// Set shader material uniforms
	// 			if (Material)
	// 			{
	// 				Shader->uniform_vec3("AmbientColor", Material->Ambient);
	// 				Shader->uniform_vec3("BaseDiffuseColor", Material->Diffuse);
	// 				Shader->uniform_vec3("SpecularColor", Material->Specular);
	//
	// 				// Bind diffuse texture to unit 0
	// 				if (Material->DiffuseTexture)
	// 				{
	// 					Material->DiffuseTexture->Bind(0);
	// 					Shader->uniform_int32("HasDiffuseTexture", 1);
	// 				}
	// 				else
	// 				{
	// 					Shader->uniform_int32("HasDiffuseTexture", 0);
	// 				}
	//
	// 			}
	//
	// 			// Setup lighting uniforms
	// 			if(Lighting)
	// 			{
	// 				Shader->uniform_vec3("LightDirection", Lighting->Light.Direction);
	// 				Shader->uniform_vec3("LightColor", Lighting->Light.Color.ToVector3());
	// 				Shader->uniform_float("LightIntensity", Lighting->Light.Intensity);
	// 			}
	//
	// 			Shader->uniformMat44("model", Model);
	// 			Shader->uniformMat44("view_proj", ViewProjection);
	//
	// 			VertArray->render(prim, StartIndex, Count);
	// 		}
	// 		Shader->unbind();
	//
	// 		++KeyItr;
	// 	}
	// }
	
	void Mesh::Reset()
	{
		// TODO: Performance: could probably be better here, do not erase elements but stomp over them instead maintaining the greatest allocated size
		Data->VertData->Vertices.Clear();
		Data->VertData->Indices.Clear();

		Data->VertData->VertexCount = 0;
		Data->VertData->IndexCount = 0;

		// Erase mesh section count/start index information.
		KeyIterator<int32, MeshSection> KeyItr = Data->Sections.CreateKeyIterator();
		while (KeyItr)
		{
			Data->Sections.get(**KeyItr)->Count = 0;
			Data->Sections.get(**KeyItr)->StartIndex = 0;

			++KeyItr;
		}
	}

	MeshData* Mesh::GetMeshData()
	{
		return Data;
	}

	MeshSection* Mesh::GetSection(int32 Index)
	{
		return Data->Sections.get(Index);
	}

	VertexArray* Mesh::GetVertexArray()
	{
		return VertArray;
	}

	BoundingBox& Mesh::GetBoundingBox()
	{
		if(!StoredBoundingBox)
		{
			StoredBoundingBox = new BoundingBox;
		}
		else
		{
			return *StoredBoundingBox;
		}
		
		for(int32 VertexIndex = 0; VertexIndex < Data->GetVertexCount(); VertexIndex++)
		{
			// Assume position is first 3 elements of every vertex
			float X = Data->GetVertexElement(VertexIndex, 0);
			float Y = Data->GetVertexElement(VertexIndex, 1);
			float Z = Data->GetVertexElement(VertexIndex, 2);

			if (X > StoredBoundingBox->MaxX)
				StoredBoundingBox->MaxX = X;
			if (X < StoredBoundingBox->MinX)
				StoredBoundingBox->MinX = X;

			if (Y > StoredBoundingBox->MaxY)
				StoredBoundingBox->MaxY = Y;
			if (Y < StoredBoundingBox->MinY)
				StoredBoundingBox->MinY = Y;

			if (Z > StoredBoundingBox->MaxZ)
				StoredBoundingBox->MaxZ = Z;
			if (Z < StoredBoundingBox->MinZ)
				StoredBoundingBox->MinZ = Z;
		}

		return *StoredBoundingBox;

	}

	/*Mesh* BuildScreenSpaceMesh1P1UV(Shader* Shader)
	{
		Mesh* ScreenSpaceMesh = new Ry::Mesh(VF1P1UV);

		Vertex1P1UV V1(-1.0f, -1.0f, 0.0f, 0, 0);
		Vertex1P1UV V2(-1.0f, 1.0f, 0.0f, 0, 1);
		Vertex1P1UV V3(1.0f, 1.0f, 0.0f, 1,1);
		Vertex1P1UV V4(1.0f, -1.0f, 0.0f, 1, 0);

		ScreenSpaceMesh->GetMeshData()->AddVertex(&V1);
		ScreenSpaceMesh->GetMeshData()->AddVertex(&V2);
		ScreenSpaceMesh->GetMeshData()->AddVertex(&V3);
		ScreenSpaceMesh->GetMeshData()->AddVertex(&V4);

		ScreenSpaceMesh->GetMeshData()->AddTriangle(2, 0, 1);
		ScreenSpaceMesh->GetMeshData()->AddTriangle(2, 3, 0);

		ScreenSpaceMesh->Update();
		ScreenSpaceMesh->GetMeshData()->SetShaderAll(Shader);

		return ScreenSpaceMesh;
	}*/

}
