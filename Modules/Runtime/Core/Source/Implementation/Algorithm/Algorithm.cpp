#include "Algorithm/Algorithm.h"

namespace Ry
{
	
	template <>
	CORE_MODULE uint32 Hash<int32>(const int32& Object)
	{
		return Object;
	}

	template <>
	CORE_MODULE uint32 Hash<uint32>(const uint32& Object)
	{
		return Object;
	}

}