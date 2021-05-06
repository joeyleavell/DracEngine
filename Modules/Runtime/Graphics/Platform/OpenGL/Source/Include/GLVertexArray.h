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

		GLVertexArray(const VertexFormat& Format);
		virtual ~GLVertexArray()= default;

		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/
		void DeleteArray() override;
		void PushVertexData(float* VertData, uint32 Verts) override;
		void PushIndexData(uint32* Indices, uint32 Count) override;
		uint32 GetVertexCount() const override;
		uint32 GetIndexCount() const override;

		const GLuint GetVaoHandle() const
		{
			return vao;
		}

		const GLuint GetIboHandle() const
		{
			return ibo;
		}

		const GLuint GetVboHandle() const
		{
			return vbo;
		}

		// virtual void push_vert_data(float* data, uint32 verts, BufferHint hint);
		// virtual void push_index_data(uint32* data, uint32 indices, BufferHint hint);
		// virtual void render(Primitive primitive) const;
		// virtual void render(Primitive primitive, int32 first, int32 count) const;
		// virtual void deleteArray();
		//
		// virtual void set_format(const VertexFormat& format);

	private:

		VertexFormat Format;


		// todo: move to gl command buffer
		//int32 PrimitiveToGL(Primitive prim) const;
		int32 HintToGL(BufferHint hint) const;

		GLuint vao;
		GLuint vbo;
		GLuint ibo;

		VertexAttribPtr* Attributes;

		int32 VertexCount;
		int32 IndexCount;
	};

}