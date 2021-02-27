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

// Detect host architecture
// Note: atm these are only for gcc/msvc
#if defined(_M_AMD64) || defined(__amd64__) // amd64/x86_64
	#define RYBUILD_Arch_x86_64
#elif defined(_M_IX86) ||defined(__i686__) // i686/x86
	#define RYBUILD_Arch_Arm
#elif defined(__arm__) || defined(_M_ARM) // arm todo: (need to take into account version)
	#define RYBUILD_Arch_Arm
#endif

// Detect host compiler/toolset
// Remember GCC can be used on windows! (MinGW)
#if defined(__GNUC__)
	#define RYBUILD_Toolset_GCC
#elif defined(_MSC_VER)
	#define RYBUILD_Toolset_MSVC
#endif

// Detect host OS
#ifdef _WIN32
	#define RYBUILD_WINDOWS	
#elif defined(unix) || defined (__unix) || defined(__unix__)

	#if defined(__APPLE__) && defined(__MACH__)
		#define RYBUILD_MAC
	#else
		#define RYBUILD_LINUX
	#endif

#endif