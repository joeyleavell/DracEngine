#pragma once

#include "Interface2/VertexArray2.h"
#include "Vertex.h"
#include "glew.h"
#include "OpenGL2Gen.h"

namespace Ry
{
	class OPENGL2_MODULE GLVertexArray2 : public VertexArray2
	{
	public:

		GLVertexArray2(const VertexFormat& Format);
		virtual ~GLVertexArray2()= default;

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
		int32 HintToGL(BufferHint2 hint) const;

		GLuint vao;
		GLuint vbo;
		GLuint ibo;

		VertexAttribPtr* Attributes;

		int32 VertexCount;
		int32 IndexCount;
	};

}