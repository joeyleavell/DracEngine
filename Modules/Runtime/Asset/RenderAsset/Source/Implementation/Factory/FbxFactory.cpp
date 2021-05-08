#pragma once

#include "Factory/FbxFactory.h"
#include "Core/Globals.h"
#include "fbxsdk.h"
#include "MeshData.h"

namespace Ry
{

	struct FBXVertex
	{
		Vector3 Vertex;
		Vector2 UV;
		Vector3 Normal;
	};

	void LoadFbxMesh(FbxMesh* Mesh)
	{
		MeshData* Dat = new MeshData;

		int32 ControlPointCount = Mesh->GetControlPointsCount();

		Ry::Map<Ry::Vertex1P1UV1N, int32> Vertices;

		Ry::ArrayList<FBXVertex> FBXVerts;

		int32 PolygonCount = Mesh->GetPolygonCount();
		for(int32 PolygonIndex = 0; PolygonIndex < PolygonCount; PolygonIndex++)
		{
			int lPolygonSize = Mesh->GetPolygonSize(PolygonIndex);

			for (int32 j = 0; j < lPolygonSize; j++)
			{
				FBXVertex ResultVertex;
				
				int ControlPointIndex = Mesh->GetPolygonVertex(PolygonIndex, j);
				if (ControlPointIndex < 0)
				{
					std::cout << "Coordinates: Invalid index found!" << std::endl;
					continue;
				}

				FbxVector4 ControlPoint = Mesh->GetControlPointAt(ControlPointIndex);
				ResultVertex.Vertex.x = ControlPoint[0];
				ResultVertex.Vertex.y = ControlPoint[1];
				ResultVertex.Vertex.z = ControlPoint[2];


				for (int l = 0; l < Mesh->GetElementVertexColorCount(); l++)
				{
					FbxGeometryElementVertexColor* leVtxc = Mesh->GetElementVertexColor(l);
					//FBXSDK_sprintf(header, 100, "            Color vertex: ");

					switch (leVtxc->GetMappingMode())
					{
					default:
						break;
					case FbxGeometryElement::eByControlPoint:
						switch (leVtxc->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							//FbxColor Color = leVtxc->GetDirectArray().GetAt(ControlPointIndex);
							break;
						case FbxGeometryElement::eIndexToDirect:
						{
							// int id = leVtxc->GetIndexArray().GetAt(ControlPointIndex);
							// FbxColor Color = leVtxc->GetDirectArray().GetAt(id);
							break;
						}
						default:
							break; // other reference modes not shown here!
						}
						break;

					case FbxGeometryElement::eByPolygonVertex:
					{
						switch (leVtxc->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						//	DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
							break;
						case FbxGeometryElement::eIndexToDirect:
						{
							//int id = leVtxc->GetIndexArray().GetAt(vertexId);
						//	DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
						}
						break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;

					case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
					case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
					case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
						break;
					}
				}

				int32 UVCount = Mesh->GetElementUVCount();

				if(UVCount > 0)
				{
					FbxGeometryElementUV* MeshUVs = Mesh->GetElementUV(0);

					FbxVector2 ResultUV;
					
					switch (MeshUVs->GetMappingMode())
					{
					default:
						break;
					case FbxGeometryElement::eByControlPoint:
						switch (MeshUVs->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							ResultUV = MeshUVs->GetDirectArray().GetAt(ControlPointIndex);
							break;
						case FbxGeometryElement::eIndexToDirect:
						{
							int UVID = MeshUVs->GetIndexArray().GetAt(ControlPointIndex);
							ResultUV = MeshUVs->GetDirectArray().GetAt(UVID);
						}
						break;
						default:
							break; // other reference modes not shown here!
						}
						break;

					case FbxGeometryElement::eByPolygonVertex:
					{
						int lTextureUVIndex = Mesh->GetTextureUVIndex(ControlPointIndex, j);
						switch (MeshUVs->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
						{
							ResultUV = MeshUVs->GetDirectArray().GetAt(lTextureUVIndex);
						}
						break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;
					case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
					case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
					case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
						break;
					}

					// Add the UV to the FBX vertex
					ResultVertex.UV.x = ResultUV[0];
					ResultVertex.UV.y = ResultUV[1];

				}

				// Add FBX vertex to result verts
				FBXVerts.Add(ResultVertex);
			}
		}

		int lMtrlCount = 0;
		FbxNode* lNode = NULL;
		lNode = Mesh->GetNode();
		if (lNode)
			lMtrlCount = lNode->GetMaterialCount();

		int32 MatElCount = Mesh->GetElementMaterialCount();

		if(MatElCount > 0)
		{
//			int ElMatCount = Mesh->;

			std::cout << MatElCount << std::endl;

			FbxGeometryElementMaterial* ElMat = Mesh->GetElementMaterial(0);
			if (ElMat)
			{

				int lMaterialCount = 0;

				if (ElMat->GetReferenceMode() == FbxGeometryElement::eDirect ||
					ElMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					lMaterialCount = lMtrlCount;
				}

				if (ElMat->GetReferenceMode() == FbxGeometryElement::eIndex ||
					ElMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					int i;

					int lIndexArrayCount = ElMat->GetIndexArray().GetCount();
					for (i = 0; i < lIndexArrayCount; i++)
					{
						int MatIndex = ElMat->GetIndexArray().GetAt(i);
						FbxSurfaceMaterial* Material = lNode->GetMaterial(MatIndex);

						std::cout << "Found material" << std::endl;
					}
				}

			}
		}
		
		Ry::Log->Logf("Control points: %d", ControlPointCount);
		for(int32 ControlPointIndex = 0; ControlPointIndex < ControlPointCount; ControlPointIndex++)
		{
		//	Mesh->GetUV
		}
	}

	void Recurse(FbxNode* Root, int32 Depth)
	{
		if(!Root)
		{
			return;
		}

		FbxNodeAttribute* NodeAttrib = Root->GetNodeAttribute();

		if (NodeAttrib)
		{
			FbxNodeAttribute::EType AttribType = NodeAttrib->GetAttributeType();

			// Process different attrib types
			if (AttribType == FbxNodeAttribute::EType::eMesh)
			{
				FbxMesh* MeshData = static_cast<FbxMesh*>(NodeAttrib);

				LoadFbxMesh(MeshData);
			}

		}
		
		int32 ChildCount = Root->GetChildCount();

		for (int32 Child = 0; Child < ChildCount; Child++)
		{
			FbxNode* ChildNode = Root->GetChild(Child);

			Recurse(ChildNode, Depth + 1);
		}
	}

	void FbxFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		FbxManager* SdkManager = FbxManager::Create();

		// Create an IOSettings object.
		// Let’s assume that mySDKManager has already been created.
		FbxIOSettings* IOSettings = FbxIOSettings::Create(SdkManager, IOSROOT);
		SdkManager->SetIOSettings(IOSettings); // Store IO settings here

		// Import options determine what kind of data is to be imported.
		// True is the default, but here we’ll set some to true explicitly, and others to false.
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, false);
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, false);
		(*(SdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

		// Create an importer
		FbxImporter* Importer = FbxImporter::Create(SdkManager, "");

		const bool ImportStat = Importer->Initialize(*Reference.GetAbsolute(), -1, SdkManager->GetIOSettings());
		if (!ImportStat)
		{
			Ry::Log->LogErrorf("Failed to import fbx scene: %s", *Reference);
			return;
		}

		int32 Major, Minor, Revision;
		Importer->GetFileVersion(Major, Minor, Revision);

		Ry::Log->Logf("FBX file version: %d.%d.%d", Major, Minor, Revision);

		// Create an FBX scene object
		FbxScene* Scene = FbxScene::Create(SdkManager, "");

		bool LoadStatus = Importer->Import(Scene);

		if(!LoadStatus)
		{
			Ry::Log->LogErrorf("Failed to import fbx scene: %s", *Reference);
		}

		FbxNode* RootNode = Scene->GetRootNode();

		if (RootNode)
		{
			// Children of root node
			int32 ChildCount = RootNode->GetChildCount();

			for(int32 Child = 0; Child < ChildCount; Child++)
			{
				Recurse(RootNode->GetChild(Child), 0);
			}
		}

	}
	
}
