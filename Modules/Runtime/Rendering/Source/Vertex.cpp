#include "Vertex.h"

namespace Ry
{

	bool Vertex1P1UV1N::operator==(const Vertex1P1UV1N& Other) const
	{
		return Position == Other.Position && UV == Other.UV && Normal == Other.Normal;
	}

}