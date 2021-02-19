#pragma once

#include "Core/Core.h"
#include "Vertex.h"

namespace Ry
{

	/**
	 * Hints that specify how the buffer is intended to be used.
	 */
	enum class RENDERING_MODULE BufferHint2
	{
		STREAM, DYNAMIC, STATIC
	};

	/**
	 * Primitives available to render the vertex array.
	 */
	// enum class RENDERING_MODULE Primitive2
	// {
	// 	LINE, TRIANGLE
	// };

	/**
	 * An abstract representation of an array of vertices stored on the graphics card.
	 * A vertex array can optionally have an internal index buffer, which stores an array of indices that indexes into the vertex array.
	 */
	class RENDERING_MODULE VertexArray2
	{
	public:

		/**
		 * Creates a new vertex array.
		 * @param maxAttribs The maximum amount of attributes that can be stored in this vertex array.
		 * @param withIndexBuffer Whether this vertex array should also have an index buffer.
		 */
		VertexArray2(const VertexFormat& Format) : VFormat(Format){}
		virtual ~VertexArray2() = default;

		/**
		 * Deletes this vertex array from GPU memory. Do not use this vertex array after calling this.
		 */
		virtual void DeleteArray() = 0;

		
		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/

		/**
		 * Pushes vertex data to the GPU.
		 * @param data The float array that contains the vertex attribute values of the vertices.
		 * @param verts The amount of vertices contained in the data array
		 * @param element_size The amount of floats in each vertex
		 * @param hint The intended usage of this buffer
		 */
		virtual void PushVertexData(float* VertData, uint32 Verts) = 0;
		
		/**
		 * Pushes index data to the GPU.
		 * @param data The array that contains the indices that index the vertex array.
		 * @param indices The amount of indices stored in the array
		 * @param hint The intended usage of this buffer
		 */
		virtual void PushIndexData(uint32* Indices, uint32 Count) = 0;

		virtual uint32 GetVertexCount() const = 0;
		virtual uint32 GetIndexCount() const = 0;

		VertexFormat& GetFormat() { return VFormat; }

	protected:

		VertexFormat VFormat;


	};
}
