#include "Vertex.h"

namespace Ry
{

	// todo: make a macro for these?
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
		return Name == other.Name && Size == other.Size;
	}

	bool Vertex1P1UV1N::operator==(const Vertex1P1UV1N& Other) const
	{
		return Position == Other.Position && UV == Other.UV && Normal == Other.Normal;
	}
	
}