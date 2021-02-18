#include "Vertex.h"

namespace Ry
{

	VertexAttrib pos = VertexAttrib("a_pos", 3);
	VertexAttrib color = VertexAttrib("a_color", 4);
	VertexAttrib uv = VertexAttrib("a_tex_coord", 2); 
	VertexAttrib normal = VertexAttrib("a_normal", 3);
	VertexAttrib tangent = VertexAttrib("a_tangent", 3);
	VertexAttrib bitangent = VertexAttrib("a_bitangent", 3);

	VertexAttrib ATTRIB_ARRAY_1P[1] = {pos};
	VertexAttrib ATTRIB_ARRAY_1P1C[2] = { pos, color};
	VertexAttrib ATTRIB_ARRAY_1P1UV[2] = { pos, uv};
	VertexAttrib ATTRIB_ARRAY_1P1UV1C[3] = { pos, uv, color};
	VertexAttrib ATTRIB_ARRAY_1P1C1UV[3] = {pos, color, uv};
	VertexAttrib ATTRIB_ARRAY_1P1UV1N[3] = {pos, uv, normal};
	VertexAttrib ATTRIB_ARRAY_1P1N1UV[3] = {pos, normal, uv};
	VertexAttrib ATTRIB_ARRAY_1P1C1N[3] = { pos, color, normal};
	VertexAttrib ATTRIB_ARRAY_1P1N1C[3] = {pos, normal, color};

	VertexFormat VF1P = VertexFormat(ATTRIB_ARRAY_1P, 1);
	VertexFormat VF1P1C = VertexFormat(ATTRIB_ARRAY_1P1C, 2);
	VertexFormat VF1P1UV = VertexFormat(ATTRIB_ARRAY_1P1UV, 2);
	VertexFormat VF1P1UV1N = VertexFormat(ATTRIB_ARRAY_1P1UV1N, 3);
	VertexFormat VF1P1N1UV = VertexFormat(ATTRIB_ARRAY_1P1N1UV, 3);
	VertexFormat VF1P1UV1C = VertexFormat(ATTRIB_ARRAY_1P1UV1C, 3);
	VertexFormat VF1P1C1UV = VertexFormat(ATTRIB_ARRAY_1P1C1UV, 3);
	VertexFormat VF1P1C1N = VertexFormat(ATTRIB_ARRAY_1P1C1N, 3);
	VertexFormat VF1P1N1C = VertexFormat(ATTRIB_ARRAY_1P1N1C, 3);

	bool VertexAttrib::operator==(const VertexAttrib& other)
	{
		return name == other.name && size == other.size;
	}

	bool Vertex1P1UV1N::operator==(const Vertex1P1UV1N& Other) const
	{
		return pos == Other.pos && tex_coord == Other.tex_coord && normal == Other.normal;
	}

	void Vertex1P::populate(float* data) const
	{
		data[0] = pos.x;
		data[1] = pos.y;
		data[2] = pos.z;
	}

	void Vertex1P1C::populate(float* data) const
	{
		Vertex1P::populate(data);

		data[3] = color.r;
		data[4] = color.g;
		data[5] = color.b;
		data[6] = color.a;
	}

	void Vertex1P1UV::populate(float* data) const
	{
		Vertex1P::populate(data);

		data[3] = tex_coord.x;
		data[4] = tex_coord.y;
	}

	void Vertex1P1UV1N::populate(float* data) const
	{
		Vertex1P1UV::populate(data);

		data[5] = normal.x;
		data[6] = normal.y;
		data[7] = normal.z;
	}

	void Vertex1P1UV1C::populate(float* data) const
	{
		Vertex1P1UV::populate(data);

		data[5] = color.r;
		data[6] = color.g;
		data[7] = color.b;
		data[8] = color.a;
	}
}