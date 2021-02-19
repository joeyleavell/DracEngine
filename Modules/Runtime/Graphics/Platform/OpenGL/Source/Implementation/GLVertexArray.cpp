#include "GLVertexArray.h"
#include <iostream>

namespace Ry
{

	GLVertexArray::GLVertexArray(const VertexFormat& format, bool withIndexBuffer):
		VertexArray(format, withIndexBuffer)
	{
		this->format = format;
		this->attribs = new VertexAttribPtr[format.attribute_count];
		this->withIndexBuffer = withIndexBuffer;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		{
			glGenBuffers(1, &vbo);

			if (withIndexBuffer)
			{
				glGenBuffers(1, &ibo);
			}

			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			int32 offset = 0;

			for (int attrib_index = 0; attrib_index < format.attribute_count; attrib_index++)
			{
				VertexAttrib& attribute = format.attributes[attrib_index];

				VertexAttribPtr attribute_ptr;
				attribute_ptr.attrib = attribute;
				attribute_ptr.offset = offset;

				attribs[attrib_index] = attribute_ptr;

				// Setup the format of this attribute.
				glVertexAttribFormat(attrib_index, attribute.Size, GL_FLOAT, GL_FALSE, offset);

				// Enable this attribute.
				glEnableVertexAttribArray(attrib_index);

				// Offset is in bytes
				offset += attribute.Size * sizeof(float);
			}

			// Set the stride for all vertex attributes.
			for (int32 i = 0; i <= format.attribute_count; i++)
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

	GLVertexArray::~GLVertexArray()
	{
	}

	void GLVertexArray::deleteArray()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		glDeleteVertexArrays(1, &vao);
	}

	void GLVertexArray::set_format(const VertexFormat& format)
	{
		VertexArray::set_format(format);

		// change vertex attribute specs in vao
	}

	int32 GLVertexArray::hint_to_gl(BufferHint hint) const
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

	int32 GLVertexArray::prim_to_gl(Primitive prim) const
	{
		int32 gl_primitive = GL_TRIANGLES;

		switch (prim)
		{
		case Primitive::TRIANGLE:
			gl_primitive = GL_TRIANGLES;
			break;
		case Primitive::LINE:
			gl_primitive = GL_LINES;
			break;
		}

		return gl_primitive;
	}

	void GLVertexArray::push_vert_data(float* data, uint32 verts, BufferHint hint)
	{
		this->verts = verts;

		glBindVertexArray(vao);
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			{
				glBufferData(GL_ARRAY_BUFFER, 4 * verts * format.element_count, data, hint_to_gl(hint));
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glBindVertexArray(0);

	}

	void GLVertexArray::push_index_data(uint32* data, uint32 indices, BufferHint hint)
	{

		glBindVertexArray(vao);
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * indices, data, hint_to_gl(hint));
			}
		}
		glBindVertexArray(0);
		this->indices = indices;

	}

	void GLVertexArray::render(Primitive primitive) const
	{
		if (withIndexBuffer)
		{
			render(primitive, 0, indices);
		}
		else
		{
			render(primitive, 0, verts);
		}
	}

	void GLVertexArray::render(Primitive primitive, int32 first, int32 count) const
	{
		glBindVertexArray(vao);
		{

			if (withIndexBuffer)
			{
				glDrawElements(prim_to_gl(primitive), count, GL_UNSIGNED_INT, reinterpret_cast<void*>(first * sizeof(int32)));
			}
			else
			{
				glDrawArrays(prim_to_gl(primitive), first, count);
			}

		}
		glBindVertexArray(0);
	}

}