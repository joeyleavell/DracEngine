#pragma once

#include "Core/Core.h"
#include "Vertex.h"

namespace Ry
{

	/**
	 * Hints that specify how the buffer is intended to be used.
	 */
	enum class RENDERINGCORE_MODULE BufferHint
	{
		STREAM, DYNAMIC, STATIC
	};

	/**
	 * Primitives available to render the vertex array.
	 */
	enum class RENDERINGCORE_MODULE Primitive
	{
		LINE, TRIANGLE
	};

	/**
	 * An abstract representation of an array of vertices stored on the graphics card.
	 * A vertex array can optionally have an internal index buffer, which stores an array of indices that indexes into the vertex array.
	 */
	class RENDERINGCORE_MODULE VertexArray
	{
	public:

		/**
		 * Creates a new vertex array.
		 * @param maxAttribs The maximum amount of attributes that can be stored in this vertex array.
		 * @param withIndexBuffer Whether this vertex array should also have an index buffer.
		 */
		VertexArray(const VertexFormat& format, bool withIndexBuffer = true):
			format(format)
		{

		}

		virtual ~VertexArray()
		{

		}

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
		virtual void push_vert_data(float* data, uint32 verts, BufferHint hint) = 0;
		
		/**
		 * Pushes index data to the GPU.
		 * @param data The array that contains the indices that index the vertex array.
		 * @param indices The amount of indices stored in the array
		 * @param hint The intended usage of this buffer
		 */
		virtual void push_index_data(uint32* data, uint32 indices, BufferHint hint) = 0;
			
		/**
		 * Renders this vertex array.
		 * @param prim The primitive to interpret the vertices as
		 */
		virtual void render(Primitive prim) const = 0;
		virtual void render(Primitive prim, int32 first, int32 count) const = 0;

		/**
		 * Deletes this vertex array from GPU memory. Do not use this vertex array after calling this.
		 */
		virtual void deleteArray() = 0;

		virtual const VertexFormat& get_format() const
		{
			return format;
		}

		virtual void set_format(const VertexFormat& format)
		{
			this->format = format;
		}

	protected:

		VertexFormat format;

	};
}
