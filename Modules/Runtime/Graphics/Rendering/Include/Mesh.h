#pragma once

#include "Core/Core.h"
#include "MeshData.h"
#include "Light.h"
#include "Vertex.h"
#include "Interface/VertexArray.h"

namespace Ry
{
	struct VertexFormat;

	struct VertexAttrib;
	class VertexArray2;
	struct BoundingBox;

	/**
	 * A mesh is a collection of vertices and indexes defining either a 3D or 2D object.
	 */
	class RENDERING_MODULE Mesh
	{
	public:

		/**
		 * Creates a blank mesh.
		 * @param maxVerts Determines how much space to allocate to store vertices
		 * @param maxIndices Determines how much space to allocate to store indices
		 * @param usage The intended usage of this mesh
		 * @param attribs The vertex attributes to use for each vertex in this mesh.
		 * @param attribCount The amount of vertex attributes
		 */
		Mesh(const VertexFormat& Format = VF1P1UV1N);
		Mesh(MeshData* Data);

		virtual ~Mesh();

		/**
		 * Frees only the resources pertaining to this mesh such as the internal vertex array and index array.
		 *
		 * The materials will have to be individually deleted.
		 */
		void DeleteMesh();

		/**
		 * Updates the GPU vertex and index data. This is the sole function that mutates mesh data on the GPU. Must be called after previously calling add_vertex and add_index
		 */
		void Update();

		/**
		 * Renders the data contained in this mesh.
		 * @prim The primitive to use for rendering the mesh.
		 */
	//	void Render(Primitive prim);
	//	void Render(Primitive prim, const Ry::Matrix4& Model, const Ry::Matrix4& ViewProjection, const SceneLighting* Lighting);

		/**
		 * Clears the vertex and index data internally stored in this mesh. Shader and material information remain, but all vertex and index information is erased.
		 */
		void Reset();

		MeshData* GetMeshData() const;

		MeshSection* GetSection(int32 Index);

		VertexArray* GetVertexArray();

		BoundingBox& GetBoundingBox();

	private:

		// Mesh data
		MeshData* Data;

		VertexArray* VertArray;
		BufferHint Hint;

		BoundingBox* StoredBoundingBox;
	};

	//Mesh* BuildScreenSpaceMesh1P1UV(Shader* Shader);
	
}
