#pragma once

#include "Core/Core.h"
#include "Data/Map.h"
#include "Data/ArrayList.h"
#include "RenderingGen.h"
#include "Vertex.h"
#include "Core/Globals.h"

namespace Ry
{
	struct Material;
	
	struct RENDERING_MODULE MeshSection
	{

		/**
		 * The ID of this mesh section.
		 */
		int32 SectionId = 0;

		/**
		 * The starting element for this section.
		 */
		int32 StartIndex = 0;

		/**
		 * How many elements are in this mesh section.
		 */
		int32 Count = 0;

		// Load materials for mesh
		Material* Mat = nullptr;
	};

	class RENDERING_MODULE MeshVertexData
	{
	public:
		/**
		 * How many vertices are in this mesh.
		 */
		int32 VertexCount;

		/**
		 * How many indices are in this mesh/
		 */
		int32 IndexCount;

		/**
		 * The format of vertices stored in this mesh.
		 */
		VertexFormat Format;

		/**
		 * Vertex information for this mesh.
		 */
		Ry::ArrayList<float> Vertices;

		/**
		 * Index information for this mesh.
		 */
		Ry::ArrayList<uint32> Indices;

		MeshVertexData()
		{
			this->VertexCount = 0;
			this->IndexCount = 0;
		}

		void AddVertex(std::initializer_list<float> Values)
		{
			for (float Val : Values)
			{
				Vertices.Add(Val);
			}

			VertexCount++;
		}

		void SetVertexElement(int32 Vertex, int32 Element, float Value)
		{
			Vertices[(uint64)Format.GetElementCount() * Vertex + Element] = Value;
		}

		float GetVertexElement(int32 Vertex, int32 Element)
		{
			return Vertices[(uint64)Format.GetElementCount() * Vertex + Element];
		}

		/**
		 * Adds a vertex to this mesh. Once finished adding vertices and indices, make sure to call update()
		 * @param vertex The vertex
		 */
		void AddVertex(const Vertex* Vertex)
		{
			// ASSUMPTION: If you have over 30 elements in your vertex you're doing something wrong.
			float data[30];
			Vertex->Pack(data);

			for (int32 i = 0; i < Format.GetElementCount(); i++)
			{
				Vertices.Add(data[i]);
			}

			VertexCount++;
		}

		void AddVertex(float* Data)
		{
			int32 data_index = 0;

			for (int32 i = 0; i < Format.NumAttributes(); i++)
			{
				const VertexAttrib& Attribute = Format.GetAttrib(i);

				for (int32 j = 0; j < Attribute.Size; j++)
				{
					Vertices.Add(Data[data_index + j]);
				}

				data_index += Attribute.Size;
			}

			VertexCount++;
		}

		void AddVertexRaw(float* Data, int32 DataSize, int32 Verts)
		{
			for (int32 Element = 0; Element < DataSize; Element++)
			{
				Vertices.Add(Data[Element]);
			}

			VertexCount += Verts;
		}

		/**
		 * @return uint32 The amount of vertices in this mesh
		 */
		uint32 GetVertexCount() const
		{
			return VertexCount;
		}

		/**
		 * @return uint32 The amount of indices in this mesh
		 */
		uint32 GetIndexCount() const
		{
			return IndexCount;
		}


	};

	class RENDERING_MODULE MeshData
	{
	public:

		/**
		 * How many sections are in this mesh.
		 */
		int32 SectionCount;

		/**
		 * A map of section IDs to their respective section.
		 */
		OAHashMap<int32, MeshSection> Sections{5};

		MeshVertexData* VertData;

		MeshData()
		{
			this->VertData = new MeshVertexData;
			this->SectionCount = 0;

			// Create default mesh section
			NewSection();
		}

		MeshData(MeshData* Other)
		{
			// Soft copy vert data (pointer), deep copy section data
			this->VertData = Other->VertData;

			this->SectionCount = Other->SectionCount;
			this->Sections = Other->Sections;
		}

		VertexFormat GetVertFormat()
		{
			return VertData->Format;
		}

		void SetVertFormat(const VertexFormat& Format)
		{
			VertData->Format = Format;
		}

		float* GetVertData()
		{
			return VertData->Vertices.GetData();
		}

		uint32* GetIndexData()
		{
			return VertData->Indices.GetData();
		}

		void AddVertex(std::initializer_list<float> Values)
		{
			VertData->AddVertex(Values);
		}

		float GetVertexElement(int32 Vertex, int32 Element)
		{
			return VertData->GetVertexElement(Vertex, Element);
		}

		/**
		 * Adds a vertex to this mesh. Once finished adding vertices and indices, make sure to call update()
		 * @param vertex The vertex
		 */
		void AddVertex(const Vertex* Vertex)
		{
			VertData->AddVertex(Vertex);
		}

		void AddVertex(float* Data)
		{
			VertData->AddVertex(Data);
		}

		void AddVertexRaw(float* Data, int32 DataSize, int32 Verts)
		{
			VertData->AddVertexRaw(Data, DataSize, Verts);
		}

		/**
		 * @return uint32 The amount of vertices in this mesh
		 */
		uint32 GetVertexCount() const
		{
			return VertData->GetVertexCount();
		}

		/**
		 * @return uint32 The amount of indices in this mesh
		 */
		uint32 GetIndexCount() const
		{
			return VertData->GetIndexCount();
		}

		/**
		 * Adds an index to this mesh. Once finished adding vertices and indices, make sure to call update()
		 * @param uint32 The mesh index
		 */
		void AddIndex(uint32 Index)
		{
			VertData->Indices.Add(Index);

			VertData->IndexCount++;
			Sections.Get(SectionCount - 1).Count++;
		}

		void AddIndexData(std::initializer_list<uint32> Input)
		{
			for (uint32 Index : Input)
			{
				VertData->Indices.Add(Index);
			}

			VertData->IndexCount += Input.size();
			Sections.Get(SectionCount - 1).Count += Input.size();
		}

		void AddIndexRaw(uint32* Buffer, uint32 Count, uint32 BaseIndex)
		{
			for (uint32 Index = 0; Index < Count; Index++)
			{
				VertData->Indices.Add(BaseIndex + Buffer[Index]);
			}

			VertData->IndexCount += Count;
			Sections.Get(SectionCount - 1).Count += Count;
		}

		/**
		 * Adds two indices to this mesh.
		 * @paramuint32 The first mesh index
		 * @param uint32 The second mesh index
		 */
		void AddLine(uint32 First, uint32 Second)
		{
			VertData->Indices.Add(First);
			VertData->Indices.Add(Second);

			VertData->IndexCount += 2;
			Sections.Get(SectionCount - 1).Count += 2;
		}

		/**
		 * Adds two indices to this mesh.
		 * @param i1 The first mesh index
		 * @param i2 The second mesh index
		 * @param i3 The third mesh index
		 */
		void AddTriangle(uint32 First, uint32 Second, uint32 Third)
		{
			VertData->Indices.Add(First);
			VertData->Indices.Add(Second);
			VertData->Indices.Add(Third);

			VertData->IndexCount += 3;
			Sections.Get(SectionCount - 1).Count += 3;
		}

		Material* GetMaterial(int32 Slot)
		{
			return Sections.Get(Slot).Mat;
		}

		void SetMaterial(int32 Slot, Material* Material)
		{
			Sections.Get(Slot).Mat = Material;
		}

		int32 GetCurrentSectionIndex() const
		{
			return SectionCount - 1;
		}

		void NewSection()
		{
			MeshSection Section;
			Section.SectionId = SectionCount;
			Section.StartIndex = SectionCount > 0 ? Sections.Get(SectionCount - 1).StartIndex + Sections.Get(SectionCount - 1).Count : 0;
			Section.Count = 0;

			Sections.Insert(SectionCount, Section);
			SectionCount++;
		}

		void CalcTangent(int32 Index0, int32 Index1, int32 Index2, Ry::Vector3& OutTangent, Ry::Vector3& OutBiTangent)
		{
			int32 PosOffset = VertData->Format.GetPosOffset();
			int32 UVOffset  = VertData->Format.GetUVOffset();

			Ry::Vector3 Pos0 = {
				VertData->GetVertexElement(Index0, PosOffset + 0),
				VertData->GetVertexElement(Index0, PosOffset + 1),
				VertData->GetVertexElement(Index0, PosOffset + 2)
			};

			Ry::Vector3 Pos1 = {
				VertData->GetVertexElement(Index1, PosOffset + 0),
				VertData->GetVertexElement(Index1, PosOffset + 1),
				VertData->GetVertexElement(Index1, PosOffset + 2)
			};

			Ry::Vector3 Pos2 = {
				VertData->GetVertexElement(Index2, PosOffset + 0),
				VertData->GetVertexElement(Index2, PosOffset + 1),
				VertData->GetVertexElement(Index2, PosOffset + 2)
			};

			Ry::Vector2 UV0 = {
			VertData->GetVertexElement(Index0, UVOffset + 0),
			VertData->GetVertexElement(Index0, UVOffset + 1)
			};

			Ry::Vector2 UV1 = {
				VertData->GetVertexElement(Index1, UVOffset + 0),
				VertData->GetVertexElement(Index1, UVOffset + 1)
			};

			Ry::Vector2 UV2 = {
				VertData->GetVertexElement(Index2, UVOffset + 0),
				VertData->GetVertexElement(Index2, UVOffset + 1)
			};

			Vector3 Edge1 = Pos1 - Pos0;
			Vector3 Edge2 = Pos2 - Pos0;
			Vector2 DeltaUV1 = UV1 - UV0;
			Vector2 DeltaUV2 = UV2 - UV0;

			float Det = 1.0f / (DeltaUV1.x * DeltaUV2.y - DeltaUV2.x * DeltaUV1.y);

			OutTangent = {
				Det * (DeltaUV2.y * Edge1.x - DeltaUV1.y * Edge2.x),
				Det * (DeltaUV2.y * Edge1.y - DeltaUV1.y * Edge2.y),
				Det * (DeltaUV2.y * Edge1.z - DeltaUV1.y * Edge2.z)
			};

			OutBiTangent = {
				Det * (-DeltaUV2.x * Edge1.x + DeltaUV1.x * Edge2.x),
				Det * (-DeltaUV2.x * Edge1.y + DeltaUV1.x * Edge2.y),
				Det * (-DeltaUV2.x * Edge1.z + DeltaUV1.x * Edge2.z)
			};

		}

		/**
		 * Calculates tangents and bitangents for mesh.
		 */
		void CalculateTangents()
		{
			int32 PosOffset = VertData->Format.GetPosOffset();
			int32 UVOffset = VertData->Format.GetUVOffset();
			int32 TangentOffset = VertData->Format.GetTangentOffset();
			int32 BiTangentOffset = VertData->Format.GetBiTangentOffset();

			// Determine where tangent/bitangent/uv/normal offsets are
			if(PosOffset < 0)
			{
				Ry::Log->LogError("MeshData did not have position attribute when calculating tangents");
				return;
			}

			if (UVOffset < 0)
			{
				Ry::Log->LogError("MeshData did not have UV attribute when calculating tangents");
				return;
			}

			if (TangentOffset < 0)
			{
				Ry::Log->LogError("MeshData did not have tangent attribute when calculating tangents");
				return;
			}

			if (BiTangentOffset < 0)
			{
				Ry::Log->LogError("MeshData did not have bitangent attribute when calculating tangents");
				return;
			}

			if(VertData->IndexCount % 3 != 0)
			{
				Ry::Log->LogError("MeshData indices was not a multiple of 3, can't generate tangents");
				return;
			}

			// Maintain each verts tangents and bitangents so we can average them at the end
			Ry::Map<int32, Ry::ArrayList<Vector3>> Tangents;
			Ry::Map<int32, Ry::ArrayList<Vector3>> BiTangents;

			for(int32 Triangle = 0; Triangle < VertData->IndexCount; Triangle += 3)
			{
				int Index0 = VertData->Indices[Triangle + 0];
				int Index1 = VertData->Indices[Triangle + 1];
				int Index2 = VertData->Indices[Triangle + 2];

				// Calculate all three tangents
				Ry::Vector3 Tangent0, BiTangent0;
				Ry::Vector3 Tangent1, BiTangent1;
				Ry::Vector3 Tangent2, BiTangent2;

				CalcTangent(Index0, Index1, Index2, Tangent0, BiTangent0);
				CalcTangent(Index1, Index0, Index2, Tangent1, BiTangent1);
				CalcTangent(Index2, Index0, Index1, Tangent2, BiTangent2);

				// Add the tangents/bi-tangents to the map
				if (!Tangents.contains(Index0))
					Tangents.insert(Index0, Ry::ArrayList<Vector3>());
				if (!Tangents.contains(Index1))
					Tangents.insert(Index1, Ry::ArrayList<Vector3>());
				if (!Tangents.contains(Index2))
					Tangents.insert(Index2, Ry::ArrayList<Vector3>());

				if (!BiTangents.contains(Index0))
					BiTangents.insert(Index0, Ry::ArrayList<Vector3>());
				if (!BiTangents.contains(Index1))
					BiTangents.insert(Index1, Ry::ArrayList<Vector3>());
				if (!BiTangents.contains(Index2))
					BiTangents.insert(Index2, Ry::ArrayList<Vector3>());

				Tangents.get(Index0)->Add(Tangent0);
				Tangents.get(Index1)->Add(Tangent1);
				Tangents.get(Index2)->Add(Tangent2);

				BiTangents.get(Index0)->Add(BiTangent0);
				BiTangents.get(Index1)->Add(BiTangent1);
				BiTangents.get(Index2)->Add(BiTangent2);

			}

			// Average out tangents/bitangents now
			int32 VertCount = VertData->VertexCount;
			for(int32 Vert = 0; Vert < VertCount; Vert++)
			{
				Ry::ArrayList<Vector3> TangentArray = *Tangents.get(Vert);
				Ry::ArrayList<Vector3> BiTangentArray = *BiTangents.get(Vert);

				Vector3 AvgTangent, AvgBiTangent;

				for(const Vector3& Tangent : TangentArray)
				{
					AvgTangent += Tangent;
				}

				for (const Vector3& BiTangent : BiTangentArray)
				{
					AvgBiTangent += BiTangent;
				}

				AvgTangent   *= 1.0f / TangentArray.GetSize();
				AvgBiTangent *= 1.0f / BiTangentArray.GetSize();

				// Set the vert tangent and bi-tangent elements
				VertData->SetVertexElement(Vert, TangentOffset + 0, AvgTangent.x);
				VertData->SetVertexElement(Vert, TangentOffset + 1, AvgTangent.y);
				VertData->SetVertexElement(Vert, TangentOffset + 2, AvgTangent.z);

				VertData->SetVertexElement(Vert, BiTangentOffset + 0, AvgBiTangent.x);
				VertData->SetVertexElement(Vert, BiTangentOffset + 1, AvgBiTangent.y);
				VertData->SetVertexElement(Vert, BiTangentOffset + 2, AvgBiTangent.z);
			}
			
		}

	};

}
