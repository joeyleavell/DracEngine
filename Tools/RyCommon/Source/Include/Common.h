#pragma once

extern bool bIsVerbose;

// For now, only GNU compilers with version of at least 8 support the legit filesystem.
// MSVC doesn't seem to support it yet, will need to add when they do.
#if __GNUC__ >= 8
	#include <filesystem>

	namespace Filesystem = std::filesystem;
#else
	#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
	#include <experimental/filesystem>
	#undef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

	namespace Filesystem = std::experimental::filesystem;
#endif


#ifdef _WIN32
	#define RYBUILD_WINDOWS
#elif defined(unix) || defined (__unix) || defined(__unix__)

	#if defined(__APPLE__) && defined(__MACH__)
		#define RYBUILD_MAC
	#else
		#define RYBUILD_LINUX
	#endif

#endif