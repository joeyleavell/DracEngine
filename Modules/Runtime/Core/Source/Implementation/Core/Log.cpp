#include "Core/Log.h"

namespace Ry
{
	Ry::String GetTimestamp(Ry::String Format)
	{
		char Buffer[1024 * 4];

		time_t Now = time(0);
		struct tm AsTime;

#ifdef MSVC_COMPILER
		localtime_s(&AsTime, &Now);
#else
		AsTime = *localtime(&Now);
#endif

		strftime(Buffer, 1024 * 4, Format.getData(), &AsTime);

		return Ry::String(Buffer);
	}
}