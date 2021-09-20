#pragma once

#include "Factory/ObjMeshFactory.h"
#include "MeshAsset.h"
#include "Factory/ObjMtlFactory.h"
#include "MaterialAsset.h"
#include "Vector.h"
#include "File/File.h"
#include "Vertex.h"
#include "MeshData.h"
#include "Profiler.h"
#include "Core/Globals.h"
#include <iostream>

namespace Ry
{

	OBJIndex ObjMeshFactory::ParseIndex(const Ry::StringView& Index)
	{
		OBJIndex Result;
		Ry::StringView* Indices = nullptr;
		int32 IndexCount = Index.split("/", &Indices);

		if(IndexCount >= 1)
		{
			// Vertex position
			Result.VertexIndex = Indices[0].to_uint32() - 1;
		}

		if(IndexCount >= 2)
		{
			// Vertex texture coord
			if(!Indices[1].IsEmpty())
			{
				Result.TexCoordIndex = Indices[1].to_uint32() - 1;
			}
		}
		
		if(IndexCount >= 3)
		{
			// Vertex normal
			if (!Indices[2].IsEmpty())
			{
				Result.NormalIndex = Indices[2].to_uint32() - 1;
			}
		}

		return Result;
	}

	int32 ObjMeshFactory::ExtractTokens(Ry::StringView* Input, int32 Size, Ry::StringView** Result)
	{
		*Result = new Ry::StringView[Size];
		int32 ValidIndex = 0;

		for(int32 Index = 0; Index < Size; Index++)
		{
			if(Input[Index].getSize() > 0)
			{
				(*Result)[ValidIndex] = Input[Index];
				//std::cout << "Extracted " << Result[ValidIndex]->getData() << std::endl;

				ValidIndex++;
			}
		}

		return ValidIndex;
	}
	
	OBJFace ObjMeshFactory::ParseFace(const Ry::StringView& First, const Ry::StringView& Second, const Ry::StringView& Third)
	{
		OBJFace Face;
		Face.First = ParseIndex(First);
		Face.Second = ParseIndex(Second);
		Face.Third = ParseIndex(Third);

		return Face;
	}

	Ry::String ObjMeshFactory::BuildName(int32 BeginToken, int32 TokenCount, Ry::StringView** Tokens)
	{
		Ry::String Name = "";
		for (int32 RemainingTokens = BeginToken; RemainingTokens < TokenCount; RemainingTokens++)
		{
			Name += (*Tokens)[RemainingTokens];

			if (TokenCount - RemainingTokens > 1)
				Name += " ";
		}

		return Name;
	}

	void ObjMeshFactory::LoadMaterialLib(const Ry::String Path, std::vector<Material*>& Materials)
	{
		// TODO: implement
		std::vector<Asset*> MaterialAssets;
		ObjMtlFactory* MtlLoader = new ObjMtlFactory;

		MtlLoader->LoadAssets(Path, MaterialAssets);

		for(Asset* MatAsset : MaterialAssets)
		{
			Materials.push_back(((MaterialAsset*)MatAsset)->GetRuntimeMaterial());
		}
	}

	void ObjMeshFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		Profiler Prof;
		Profiler GenerateProf;
		Profiler FaceParse;
		Profiler SplitProf;
		Profiler DeleteProf;
		Profiler LoadString;

		float TotalSplit = 0.0f;
		float TotalDelete = 0.0f;

		Ry::MeshData* MeshData = new Ry::MeshData;
		MeshData->SetVertFormat(Ry::VF_P_UV_N_T_Bi);

		std::vector<Ry::Vector3> OBJVertices;
		std::vector<Ry::Vector3> OBJNormals;
		std::vector<Ry::Vector2> OBJTexCoords;
		//std::vector<Ry::Vector2> OBJFacesNoObject; // OBJ faces without an object attached to them

		// Stack of objects
		std::vector<OBJMaterialGroup*> MaterialGroups;

		// Loaded materials from material libraries
		std::vector<Material*> Materials;

		Prof.start();

		StringView* OBJLines = nullptr;
		
		LoadString.start();
		Ry::String OBJFile = Ry::File::LoadFileAsString2(Reference.GetAbsolute());
		LoadString.end();

		SplitProf.start();
		int32 LineCount = OBJFile.split("\n", &OBJLines);
		SplitProf.end();
		TotalSplit += SplitProf.get_duration_ms();

		Ry::Material* CurrentMaterial = nullptr;

		for(uint32 Line = 0; Line < (uint32) LineCount; Line++)
		{
			Ry::StringView& OBJLine = OBJLines[Line];

			// Split OBJ line on standard space delimiter
			Ry::StringView* RawTokens = nullptr;

			SplitProf.start();
			int32 RawTokenCount = OBJLine.split(" ", &RawTokens);
			SplitProf.end();
			TotalSplit += SplitProf.get_duration_ms();

			Ry::StringView* ValidTokens = nullptr;
			int32 ValidTokenCount = ExtractTokens(RawTokens, RawTokenCount, &ValidTokens);

			if(ValidTokenCount > 0)
			{
				Ry::StringView& Command = ValidTokens[0];

				if(Command == "v")
				{
					if(ValidTokenCount == 4)
					{
						float X = ValidTokens[1].to_float();
						float Y = ValidTokens[2].to_float();
						float Z = ValidTokens[3].to_float();
						OBJVertices.push_back(Ry::Vector3(X, Y, Z));
					}
					else
					{
						Ry::Log->LogError("OBJ importer does not currently support homogenous component.");
						return;
					}
				}

				if(Command == "vt")
				{
					float U = ValidTokens[1].to_float();
					float V = ValidTokens[2].to_float();
					OBJTexCoords.push_back(Ry::Vector2(U, V));
				}

				if(Command == "vn")
				{
					float X = ValidTokens[1].to_float();
					float Y = ValidTokens[2].to_float();
					float Z = ValidTokens[3].to_float();
					OBJNormals.push_back(Ry::Vector3(X, Y, Z));
				}

				if(Command == "f")
				{
					FaceParse.start();
					OBJFace Face;
					Face.First = ParseIndex(ValidTokens[1]);
					Face.Second = ParseIndex(ValidTokens[2]);
					Face.Third = ParseIndex(ValidTokens[3]);

					if(ValidTokenCount == 5)
					{
						Face.Fourth = ParseIndex(ValidTokens[4]);
						Face.IsTriangle = false;
					}

					// Create default object if no objects are present
					if(MaterialGroups.empty())
					{
						OBJMaterialGroup* NewGroup  = new OBJMaterialGroup;
						NewGroup->Mat = nullptr;
						MaterialGroups.push_back(NewGroup);
					}

					// Add face to most recently added object
					MaterialGroups[MaterialGroups.size() - 1]->Faces.push_back(Face);
					FaceParse.end();
				}

				if(Command == "o")
				{
					// Not supported
				}

				if(Command == "mtllib")
				{
					// Load material assets
					Ry::String ObjRoot = Ry::File::GetParentPath(Reference.GetVirtual());
					Ry::String MtlPath = Ry::File::Join(ObjRoot, ValidTokens[1]);
					MtlPath.Replace('\\', '/');
					
					LoadMaterialLib(MtlPath, Materials);
				}

				if(Command == "usemtl")
				{
					// Build the material name
					Ry::String MaterialName = BuildName(1, ValidTokenCount, &ValidTokens);

					bool FoundMaterial = false;
					// Find the referenced material
					for(Material* Mat : Materials)
					{
						if(Mat->Name == MaterialName)
						{
							OBJMaterialGroup* NewGroup = new OBJMaterialGroup;
							NewGroup->Mat = Mat;

							MaterialGroups.push_back(NewGroup);

							FoundMaterial = true;
							break;
						}
					}

					if(!FoundMaterial)
					{
						Ry::Log->LogError("Could not find material " + MaterialName);
						return;
					}
				}

			}
		}

		GenerateProf.start();
		// Compile into vertices and faces
		Ry::Map<Ry::Vertex1P1UV1N, uint32> Vertices;

		for(int32 MatGroupIndex = 0; MatGroupIndex < MaterialGroups.size(); MatGroupIndex++)
		{
			OBJMaterialGroup* MatGroup = MaterialGroups[MatGroupIndex];

			// Create new section after skipping first group, since a default section is already created.
			if (MatGroupIndex > 0)
				MeshData->NewSection();

			MeshData->SetMaterial(MatGroupIndex, MatGroup->Mat);
			//MeshData->SetShader(0, nullptr); // TODO: Set shader here

			for(int32 GroupFaceIndex = 0; GroupFaceIndex < MatGroup->Faces.size(); GroupFaceIndex++)
			{
				OBJFace GroupFace = MatGroup->Faces.at(GroupFaceIndex);
				
				Ry::Vertex1P1UV1N FirstVertex, SecondVertex, ThirdVertex, FourthVertex;
				
				FirstVertex.Position = OBJVertices.at(GroupFace.First.VertexIndex);
				SecondVertex.Position = OBJVertices.at(GroupFace.Second.VertexIndex);
				ThirdVertex.Position = OBJVertices.at(GroupFace.Third.VertexIndex);
				
				if(!OBJTexCoords.empty())
				{
					FirstVertex.UV = OBJTexCoords.at(GroupFace.First.TexCoordIndex);
					SecondVertex.UV = OBJTexCoords.at(GroupFace.Second.TexCoordIndex);
					ThirdVertex.UV = OBJTexCoords.at(GroupFace.Third.TexCoordIndex);
					if (!GroupFace.IsTriangle)
					{
						FourthVertex.UV = OBJTexCoords.at(GroupFace.Fourth.TexCoordIndex);
					}
				}

				if(!OBJNormals.empty())
				{
					FirstVertex.Normal = OBJNormals.at(GroupFace.First.NormalIndex);
					SecondVertex.Normal = OBJNormals.at(GroupFace.Second.NormalIndex);
					ThirdVertex.Normal = OBJNormals.at(GroupFace.Third.NormalIndex);
					if(!GroupFace.IsTriangle)
					{
						FourthVertex.Normal = OBJNormals.at(GroupFace.Fourth.NormalIndex);
					}
				}


				if(!GroupFace.IsTriangle)
				{
					FourthVertex.Position = OBJVertices.at(GroupFace.Fourth.VertexIndex);
				}

				uint32 FirstIndex, SecondIndex, ThirdIndex, FourthIndex;

				if(Vertices.contains(FirstVertex))
				{
					FirstIndex = *Vertices.get(FirstVertex);
				}
				else
				{
					FirstIndex = MeshData->GetVertexCount();
					Vertices.insert(FirstVertex, FirstIndex);
					MeshData->AddVertex(&FirstVertex);
				}

				if (Vertices.contains(SecondVertex))
				{
					SecondIndex = *Vertices.get(SecondVertex);
				}
				else
				{
					SecondIndex = MeshData->GetVertexCount();
					Vertices.insert(SecondVertex, SecondIndex);
					MeshData->AddVertex(&SecondVertex);
				}

				if (Vertices.contains(ThirdVertex))
				{
					ThirdIndex = *Vertices.get(ThirdVertex);
				}
				else
				{
					ThirdIndex = MeshData->GetVertexCount();
					Vertices.insert(ThirdVertex, ThirdIndex);
					MeshData->AddVertex(&ThirdVertex);
				}

				MeshData->AddTriangle(FirstIndex, SecondIndex, ThirdIndex);

				// Add the extra triangle since this is a quad face mesh
				if(!GroupFace.IsTriangle)
				{
					if (Vertices.contains(FourthVertex))
					{
						FourthIndex = *Vertices.get(FourthVertex);
					}
					else
					{
						FourthIndex = MeshData->GetVertexCount();
						Vertices.insert(FourthVertex, FourthIndex);
						MeshData->AddVertex(&FourthVertex);
					}

					MeshData->AddTriangle(ThirdIndex, FourthIndex, FirstIndex);
				}

			}

		}

		GenerateProf.end();

		Prof.end();
		float LoadTime = Prof.get_duration_ms();

		//std::cout << "Total=" << LoadTime << " split%=" << (TotalSplit / LoadTime) << std::endl;
		//std::cout << "generate%=" << (GenerateProf.get_duration_ms() / LoadTime) << " loadstring%=" << (LoadString.get_duration_ms() / LoadTime) << std::endl;
		//std::cout << "delete%=" << (TotalDelete / LoadTime) << " loadstring%=" << (LoadString.get_duration_ms() / LoadTime) << std::endl;

		// Calculate tangents at end
		MeshData->CalculateTangents();
		
		AssetDst.push_back(new MeshAsset(MeshData));
	}

}
