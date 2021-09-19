#include "Core/Memory/Memory.h"

namespace Ry
{
	void MemCpy(void* Dest, uint64 DstSize, const void* Src, uint64 SrcSize)
	{
	#ifdef MSVC_COMPILER
		memcpy_s(Dest, DstSize, Src, SrcSize);
	#else
		memcpy(Dest, Src, DstSize < SrcSize ? DstSize : SrcSize);
	#endif

	}

	void MemSet(void* Dst, uint64 DstSize, int32 Value)
	{
		memset(Dst, Value, DstSize);
	}
}