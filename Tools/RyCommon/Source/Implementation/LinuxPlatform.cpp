#include "Platform.h"
#include "PlatformDetection.h"

#ifdef RBUILD_HOST_OS_LINUX
std::string GetModulePath()
{
	if (ModulePathCache.empty())
	{
		constexpr int BUF_SIZE = 1024;
		char Buffer[BUF_SIZE];

		memset(Buffer, 0, sizeof(Buffer));

		if (readlink("/proc/self/exe", Buffer, BUF_SIZE - 1) < 0)
		{
			// Error
			std::cerr << "Error getting /proc/self/exe" << std::endl;
		}

		ModulePathCache = Buffer;
	}

	return ModulePathCache;
}
#endif