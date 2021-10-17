#pragma once

#include <mach-o/dyld.h>
#include <cstdint>
#include "CoreGen.h"

/************************************************************************/
/* Define types                                                         */
/************************************************************************/
typedef std::int64_t int64;
typedef std::int32_t int32;
typedef std::int16_t int16;
typedef std::int8_t int8;
typedef std::uint64_t uint64;
typedef std::uint32_t uint32;
typedef std::uint16_t uint16;
typedef std::uint8_t uint8;

// TODO: CHANGE THIS
// The engine itself does not need to do host detection - that is the job of the build tool
/************************************************************************/
/* Platform implementation                                              */
/************************************************************************/
#if defined(_WIN32) || defined(_WIN64)
	#define WINDOWS_IMPLEMENTATION
	#include "Windows/WindowsProcess.h"
	#include "Windows/WindowsPlatform.h"
	#include "Windows/WindowsTypes.h"
#elif defined(__ANDROID__)
	#define ANDROID_IMPLEMENTATION
#elif defined(__linux__) || defined (__APPLE__)
	#define LINUX_IMPLEMENTATION
	#include "Linux/LinuxPlatform.h"
	#include "Linux/LinuxProcess.h"
#elif defined(__APPLE__)
	#define MAX_IMPLEMENTATION
#elif defined(__unix__)
	#define UNIX_IMPLEMENTATION
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
	#define MINGW_COMPILER
#endif

#if defined(__GNUG__)
	#define GCC_COMPILER
#endif

#if defined(_MSC_VER)
	#define MSVC_COMPILER
#endif

namespace Ry
{
	/**
	 * All supported rendering platforms.
	 */
	enum class RenderingPlatform
	{
		OpenGL,
		Vulkan
	};

}
