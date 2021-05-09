#include "GLVertexArray.h"
#include <iostream>

namespace Ry
{

	GLVertexArray::GLVertexArray(const VertexFormat& Format):
	VertexArray(Format)
	{
		this->Attributes = new VertexAttribPtr[Format.NumAttributes()];

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		{
			glGenBuffers(1, &vbo);

			// Always generate an index buffer
			glGenBuffers(1, &ibo);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			int32 offset = 0;

			for (int attrib_index = 0; attrib_index < Format.NumAttributes(); attrib_index++)
			{
				const VertexAttrib& attribute = Format.GetAttrib(attrib_index);

				VertexAttribPtr attribute_ptr;
				attribute_ptr.attrib = attribute;
				attribute_ptr.offset = offset;

				Attributes[attrib_index] = attribute_ptr;

				// Setup the format of this attribute.
				glVertexAttribFormat(attrib_index, attribute.Size, GL_FLOAT, GL_FALSE, offset);

				// Enable this attribute.
				glEnableVertexAttribArray(attrib_index);

				// Offset is in bytes
				offset += attribute.Size * sizeof(float);
			}

			// Set the stride for all vertex attributes.
			for (int32 i = 0; i <= Format.NumAttributes(); i++)
			{
				glBindVertexBuffer(i, vbo, 0, offset);
			}


// 			for (int32 i = 0; i < format.attribute_count; i++)
// 			{
// 				VertexAttrib& attrib = format.attributes[i];
// 				glVertexAttribPointer(i, attrib.size, GL_FLOAT, GL_FALSE, offset, reinterpret_cast<const GLvoid*>(attribs[i].offset));
// 			}

		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void GLVertexArray::DeleteArray()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		glDeleteVertexArrays(1, &vao);
	}

	void GLVertexArray::PushVertexData(float* VertData, uint32 Verts)
	{
		this->VertexCount = Verts;

		BufferHint DataHint = BufferHint::STATIC;

		glBindVertexArray(vao);
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			{
				glBufferData(GL_ARRAY_BUFFER, 4 * Verts * VFormat.ElementCount, VertData, HintToGL(DataHint));
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);
	}

	void GLVertexArray::PushIndexData(uint32* Indices, uint32 Count)
	{
		this->IndexCount = Count;

		BufferHint DataHint = BufferHint::STATIC;

		glBindVertexArray(vao);
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * Count, Indices, HintToGL(DataHint));
			}
		}
		glBindVertexArray(0);
	}

	uint32 GLVertexArray::GetVertexCount() const
	{
		return VertexCount;
	}

	uint32 GLVertexArray::GetIndexCount() const
	{
		return IndexCount;
	}

	int32 GLVertexArray::HintToGL(BufferHint hint) const
	{
		switch (hint)
		{
		case BufferHint::DYNAMIC:
			return GL_DYNAMIC_DRAW;
			break;
		case BufferHint::STREAM:
			return GL_STREAM_DRAW;
			break;
		case BufferHint::STATIC:
			return GL_STREAM_DRAW;
		}

		return -1;
	}

	// void GLVertexArray::render(Primitive primitive) const
	// {
	// 	if (withIndexBuffer)
	// 	{
	// 		render(primitive, 0, indices);
	// 	}
	// 	else
	// 	{
	// 		render(primitive, 0, verts);
	// 	}
	// }

	// void GLVertexArray::render(Primitive primitive, int32 first, int32 count) const
	// {

	// }

}