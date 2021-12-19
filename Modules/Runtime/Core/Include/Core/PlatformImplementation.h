#pragma once

#include <cstdint>

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

/************************************************************************/
/* Platform implementation                                              */
/************************************************************************/
// TODO: when generating project intellisense data, correctly add these macros
#if defined(RBUILD_TARGET_OS_WINDOWS)
	#include "Windows/WindowsPlatform.h"
#elif defined(RBUILD_TARGET_OS_LINUX) || defined(RBUILD_TARGET_OS_OSX)
	// Generic posix implementations
	#include "Posix/PosixPlatform.h"

	#if defined(RBUILD_TARGET_OS_OSX)
		#include "OSX/OSXPlatform.h" 	// OSX specific implementations
	#elif defined(RBUILD_TARGET_OS_LINUX)
		#include "Linux/LinuxPlatform.h" // Linux specific implementations
	#endif

#elif defined(__ANDROID__)
	// TODO: Android
#elif defined(__unix__)
	// TODO: Other unicies
#else
	#define EXPORT_ONLY
	#define IMPORT_ONLY
#endif