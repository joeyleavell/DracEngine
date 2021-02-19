#pragma once

#include "Core/Core.h"
#include "Data/Map.h"
#include "Data/ArrayList.h"
#include <vector>
#include "RenderingGen.h"

namespace Ry
{
	class Shader;
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

		// Load appropriate shader for mesh depending on attributes, materials, and provided parameters.
		Shader* Shad = nullptr;
	};

	class RENDERING_MODULE MeshData
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
		 * How many sections are in this mesh.
		 */
		int32 SectionCount;

		/**
		 * A map of section IDs to their respective section.
		 */
		Map<int32, MeshSection> Sections;

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

		MeshData()
		{
			this->VertexCount = 0;
			this->IndexCount = 0;
			this->SectionCount = 0;

			// Create default mesh section
			NewSection();
		}

		Material* GetMaterial(int32 Slot)
		{
			return Sections.get(Slot)->Mat;
		}

		void SetMaterial(int32 Slot, Material* Material)
		{
			Sections.get(Slot)->Mat = Material;
		}

		Shader* GetShader(int32 MeshSlot)
		{
			return Sections.get(MeshSlot)->Shad;
		}

		void SetShaderAll(Shader* Shader)
		{
			KeyIterator<int32, MeshSection> KeyItr = Sections.CreateKeyIterator();

			while (KeyItr)
			{
				Sections.get(**KeyItr)->Shad = Shader;
				++KeyItr;
			}
		}

		float GetVertexElement(int32 Vertex, int32 Element)
		{
			return Vertices[(uint64) Format.element_count * Vertex + Element];
		}

		/**
		 * Sets the shader that is used to render this mesh.
		 * @param shader The shader
		 */
		void SetShader(int32 Slot, Shader* Shader)
		{
			Sections.get(Slot)->Shad = Shader;
		}

		int32 GetCurrentSectionIndex() const
		{
			return SectionCount - 1;
		}

		void AddVertex(std::initializer_list<float> Values)
		{
			for(float Val : Values)
			{
				Vertices.Add(Val);
			}

			VertexCount++;
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

			for (int32 i = 0; i < Format.element_count; i++)
			{
				Vertices.Add(data[i]);
			}

			VertexCount++;
		}

		void AddVertex(float* Data)
		{
			int32 data_index = 0;

			for (int32 i = 0; i < Format.attribute_count; i++)
			{
				VertexAttrib& Attribute = Format.attributes[i];

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
		 * Adds an index to this mesh. Once finished adding vertices and indices, make sure to call update()
		 * @param uint32 The mesh index
		 */
		void AddIndex(uint32 Index)
		{
			Indices.Add(Index);

			IndexCount++;
			Sections.get(SectionCount - 1)->Count++;
		}

		void AddIndexData(std::initializer_list<uint32> Input)
		{
			for(uint32 Index : Input)
			{
				Indices.Add(Index);
			}

			IndexCount += Input.size();
			Sections.get(SectionCount - 1)->Count += Input.size();
		}

		void AddIndexRaw(uint32* Buffer, uint32 Count, uint32 BaseIndex)
		{
			for(uint32 Index = 0; Index < Count; Index++)
			{
				Indices.Add(BaseIndex + Buffer[Index]);
			}

			IndexCount += Count;
			Sections.get(SectionCount - 1)->Count += Count;
		}

		/**
		 * Adds two indices to this mesh.
		 * @paramuint32 The first mesh index
		 * @param uint32 The second mesh index
		 */
		void AddLine(uint32 First, uint32 Second)
		{
			Indices.Add(First);
			Indices.Add(Second);

			IndexCount += 2;
			Sections.get(SectionCount - 1)->Count += 2;
		}

		/**
		 * Adds two indices to this mesh.
		 * @param i1 The first mesh index
		 * @param i2 The second mesh index
		 * @param i3 The third mesh index
		 */
		void AddTriangle(uint32 First, uint32 Second, uint32 Third)
		{
			Indices.Add(First);
			Indices.Add(Second);
			Indices.Add(Third);

			IndexCount += 3;
			Sections.get(SectionCount - 1)->Count += 3;
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

		void NewSection()
		{
			MeshSection Section;
			Section.SectionId = SectionCount;
			Section.StartIndex = SectionCount > 0 ? Sections.get(SectionCount - 1)->StartIndex + Sections.get(SectionCount - 1)->Count : 0;
			Section.Count = 0;

			Sections.insert(SectionCount, Section);
			SectionCount++;
		}

	};
}
