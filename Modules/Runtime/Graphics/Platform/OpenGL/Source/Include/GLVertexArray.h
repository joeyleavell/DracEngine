#pragma once

#include "Interface/VertexArray.h"
#include "Vertex.h"
#include "glew.h"
#include "OpenGLGen.h"

namespace Ry
{
	class OPENGL_MODULE GLVertexArray : public VertexArray
	{
	public:

		GLVertexArray(const VertexFormat& format, bool withIndexBuffer = false);
		virtual ~GLVertexArray();

		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/

		virtual void push_vert_data(float* data, uint32 verts, BufferHint hint);
		virtual void push_index_data(uint32* data, uint32 indices, BufferHint hint);
		virtual void render(Primitive primitive) const;
		virtual void render(Primitive primitive, int32 first, int32 count) const;
		virtual void deleteArray();

		virtual void set_format(const VertexFormat& format);

	private:

		int32 hint_to_gl(BufferHint hint) const;
		int32 prim_to_gl(Primitive prim) const;

		GLuint vao;
		GLuint vbo;
		GLuint ibo;

		VertexAttribPtr* attribs;
		bool withIndexBuffer;

		int32 verts;
		int32 indices;
	};

}