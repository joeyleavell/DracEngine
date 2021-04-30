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

	VertexFormat VF1P = VertexFormat({pos});
	VertexFormat VF1P1C = VertexFormat({pos, color});
	VertexFormat VF1P1UV = VertexFormat({pos, uv});
	VertexFormat VF1P1UV1N = VertexFormat({pos, uv, normal});
	VertexFormat VF1P1N1UV = VertexFormat({pos, normal, uv});
	VertexFormat VF1P1UV1C = VertexFormat({pos, uv, color});
	VertexFormat VF1P1C1UV = VertexFormat({pos, color, uv});
	VertexFormat VF1P1C1N = VertexFormat({pos, color, normal});
	VertexFormat VF1P1N1C = VertexFormat({pos, normal, color});

	bool VertexAttrib::operator==(const VertexAttrib& other)
	{
		return Name == other.Name && Size == other.Size;
	}

	bool Vertex1P1UV1N::operator==(const Vertex1P1UV1N& Other) const
	{
		return Position == Other.Position && UV == Other.UV && Normal == Other.Normal;
	}
	
}