#include "VertexFormat.h"

namespace Ry
{

	// todo: make a macro for these?
	VertexAttrib pos = VertexAttrib("VertPos", 3);
	VertexAttrib color = VertexAttrib("VertColor", 4);
	VertexAttrib uv = VertexAttrib("VertUV", 2);
	VertexAttrib normal = VertexAttrib("VertNormal", 3);
	VertexAttrib tangent = VertexAttrib("VertTangent", 3);
	VertexAttrib bitangent = VertexAttrib("VertBiTangent", 3);

	VertexFormat VF1P = VertexFormat({ pos });
	VertexFormat VF1P1C = VertexFormat({ pos, color });
	VertexFormat VF1P1UV = VertexFormat({ pos, uv });
	VertexFormat VF1P1UV1N = VertexFormat({ pos, uv, normal });
	VertexFormat VF1P1N1UV = VertexFormat({ pos, normal, uv });
	VertexFormat VF1P1UV1C = VertexFormat({ pos, uv, color });
	VertexFormat VF1P1C1UV = VertexFormat({ pos, color, uv });
	VertexFormat VF1P1C1N = VertexFormat({ pos, color, normal });
	VertexFormat VF1P1N1C = VertexFormat({ pos, normal, color });
	VertexFormat VF_P_UV_N_T_Bi = VertexFormat({ pos, uv, normal, tangent, bitangent });

	bool VertexAttrib::operator==(const VertexAttrib& other)
	{
		return Name == other.Name && Size == other.Size;
	}

}