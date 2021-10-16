#pragma once

// Detect host architecture
#if defined(__aarch64__) // arm64
    #define RBUILD_HOST_ARCH_ARM64
#elif defined(_M_AMD64) || defined(__amd64__) // amd64/x86_64
    #define RBUILD_HOST_ARCH_X64
#elif defined(_M_IX86) || defined(__i686__) // i686/x86
	#define RBUILD_HOST_ARCH_X86
	#error 32 bit host platform not supported
#elif defined(__arm__) || defined(_M_ARM) // arm todo: (need to take into account version)
    #define RBUILD_HOST_ARCH_ARM
#else
    #error Host architecture not supported, must be one of arm64, x64, or arm
#endif

// Detect host compiler/toolset
// Note: MinGW defines GNUC
#if defined(__GNUC__)
    #define RBUILD_HOST_TOOLSET_GCC
#elif defined(_MSC_VER)
    #define RBUILD_HOST_TOOLSET_MSVC
#else
    #error Compiler not supported, must be one of GNU or MSVC
#endif

// Detect host OS
#ifdef _WIN32
    #define RBUILD_HOST_OS_WINDOWS
#elif defined(__linux__)
    #define RBUILD_HOST_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define RBUILD_HOST_OS_OSX
//#elif defined(unix) || defined (__unix) || defined(__unix__) TODO: other unicies
#else
    #error Host operating system not supported, must be one of Windows, Linux, or OSX
#endif