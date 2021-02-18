#include "Core/Core.h"

#ifdef LINUX_IMPLEMENTATION
#include "Core/String.h"
#include "Core/PlatformProcess.h"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string.h>

namespace Ry
{
	static std::string ModulePathCache;

	void* GetPlatformModuleHandle()
	{
		return nullptr;
	}

	Ry::String GetPlatformModuleParentPath()
	{
		return "";
	}

	Ry::String GetPlatformModulePath()
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

		return ModulePathCache.c_str();
	}

}

#endif