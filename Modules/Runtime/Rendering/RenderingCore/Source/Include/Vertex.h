#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "RyMath.h"
#include "RenderingCoreGen.h"
#include "Algorithm/Algorithm.h"

namespace Ry
{

	/**
	 * Simple vertex storing only position.
	 */
	struct RENDERINGCORE_MODULE Vertex
	{
		virtual void populate(float* data) const = 0;
	};

	struct RENDERINGCORE_MODULE Vertex1P : public Vertex
	{
		Vector3 pos;

		Vertex1P():
			Vertex1P(0.0f, 0.0f, 0.0f)
		{

		}

		Vertex1P(float x, float y, float z)
		{
			pos.x = x;
			pos.y = y;
			pos.z = z;
		}

		virtual void populate(float* data) const;
	};

	/**
	 * Vertex storing position and color attributes.
	 */
	struct RENDERINGCORE_MODULE Vertex1P1C : public Vertex1P
	{
		Vector4 color;

		Vertex1P1C():
			Vertex1P1C(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
		{

		}

		Vertex1P1C(float x, float y, float z, float r, float g, float b, float a) :
			Vertex1P(x, y, z)
		{
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
		}

		virtual void populate(float* data) const;
	};

	/**
	 * Vertex storing position and texture coordinate attributes.
	 */
	struct RENDERINGCORE_MODULE Vertex1P1UV : public Vertex1P
	{
		Vector2 tex_coord;

		Vertex1P1UV():
			Vertex1P1UV(0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
		{

		}

		Vertex1P1UV(float x, float y, float z, float u, float v) :
			Vertex1P(x, y, z)
		{
			tex_coord.x = u;
			tex_coord.y = v;
		}

		virtual void populate(float* data) const;
	};

	/**
	 * Vertex storing position, texture coordinate, and normal attributes. This is probably the most common vertex storage container.
	 */
	struct RENDERINGCORE_MODULE Vertex1P1UV1N : public Vertex1P1UV
	{
		Vector3 normal;

		Vertex1P1UV1N() :
			Vertex1P1UV1N(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
		{

		}

		Vertex1P1UV1N(float x, float y, float z, float u, float v, float nx, float ny, float nz) :
			Vertex1P1UV(x, y, z, u, v)
		{
			normal.x = nx;
			normal.y = ny;
			normal.z = nz;
		}

		bool operator==(const Vertex1P1UV1N& Other) const;

		virtual void populate(float* data) const;
	};

	/**
	 * Vertex storing position, texture coordinate, and color attributes.
	 */
	struct RENDERINGCORE_MODULE Vertex1P1UV1C : public Vertex1P1UV
	{
		Vector4 color;

		Vertex1P1UV1C():
			Vertex1P1UV1C(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
		{

		}

		Vertex1P1UV1C(float x, float y, float z, float u, float v, float r, float g, float b, float a) :
			Vertex1P1UV(x, y, z, u, v)
		{
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
		}

		virtual void populate(float* data) const;
	};

	/**
	 * Defines meta information about a vertex attribute. A vertex attribute is an atomic element of a vertex.
	 */
	struct RENDERINGCORE_MODULE VertexAttrib
	{
		/**
		 * The name of the vertex attribute.
		 */
		String name;

		/**
		 * The amount of elements in the vertex attribute, usually floats.
		 */
		int32 size;

		VertexAttrib()
		{
			name = "";
			size = 0;
		}

		VertexAttrib(const String& n, int32 si)
		{
			name = n;
			size = si;
		}

		bool operator==(const VertexAttrib& other);
	};

	struct RENDERINGCORE_MODULE VertexFormat
	{
		VertexAttrib* attributes;
		int32 attribute_count;
		int32 element_count;

		VertexFormat():
			VertexFormat(nullptr, 0)
		{

		}

		VertexFormat(VertexAttrib* attributes, int32 attrib_count) :
			attributes(attributes),
			attribute_count(attrib_count),
			element_count(0)
		{
			for (int32 i = 0; i < attribute_count; i++)
			{
				element_count += attributes[i].size;
			}
		}

		VertexFormat(const VertexFormat& Ref)
		{
			this->attributes = Ref.attributes;
			this->attribute_count = Ref.attribute_count;
			this->element_count = Ref.element_count;
		}

		VertexFormat(VertexFormat&& Forward) noexcept
		{
			this->attributes = Forward.attributes;
			this->attribute_count = Forward.attribute_count;
			this->element_count = Forward.element_count;
		}

		VertexFormat& operator=(const VertexFormat& Ref)
		{
			if (this == &Ref)
				return *this;
			
			this->attributes = Ref.attributes;
			this->attribute_count = Ref.attribute_count;
			this->element_count = Ref.element_count;

			return *this;
		}
	};

	/**
	* Stores a vertex attribute with its relative offset in a series of vertex attributes.
	*/
	struct RENDERINGCORE_MODULE VertexAttribPtr
	{
		VertexAttrib attrib;
		int32 offset;
	};

	extern RENDERINGCORE_MODULE VertexAttrib pos;
	extern RENDERINGCORE_MODULE VertexAttrib color;
	extern RENDERINGCORE_MODULE VertexAttrib uv;
	extern RENDERINGCORE_MODULE VertexAttrib normal;
	extern RENDERINGCORE_MODULE VertexAttrib tangent;
	extern RENDERINGCORE_MODULE VertexAttrib bitangent;

	// position
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P[1];
	
	// position, color
	// position, uv
	// position, normal
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1C[2];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1UV[2];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1N[2];

	// position, uv, color
	// position, color, uv
	// position, uv, normal
	// position, normal, uv
	// position, color, normal
	// position, normal, color
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1UV1C[3];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1C1UV[3];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1UV1N[3];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1N1UV[3];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1C1N[3];
	extern RENDERINGCORE_MODULE VertexAttrib ATTRIB_ARRAY_1P1N1C[3];

	extern RENDERINGCORE_MODULE VertexFormat VF1P;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1C;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1UV;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1UV1C;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1C1UV;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1UV1N;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1N1UV;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1C1N;
	extern RENDERINGCORE_MODULE VertexFormat VF1P1N1C;

	template <>
	EXPORT_ONLY inline uint32 Hash(const Ry::Vertex1P1UV1N& Vert)
	{
		float Data[30];
		Vert.populate(Data);

		uint32 Seed = 31;

		for (int32 El = 0; El < 8; El++)
		{
			Seed ^= std::hash<float>{}(Data[El]) + 0x9e3779b9;// +(Seed << 6) + (Seed >> 2);
		}

		return Seed;
	}

}