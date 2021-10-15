#pragma once


// Detect host architecture
// Note: atm these are only for gcc/msvc
#if defined(_M_AMD64) || defined(__amd64__) // amd64/x86_64
    #define RYBUILD_Arch_x86_64
#elif defined(_M_IX86) ||defined(__i686__) // i686/x86
    #define RYBUILD_Arch_x86
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
#elif defined(__linux__)
    #define RYBUILD_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define RYBUILD_OSX
#elif defined(unix) || defined (__unix) || defined(__unix__)
    // other unix OSs
#endif