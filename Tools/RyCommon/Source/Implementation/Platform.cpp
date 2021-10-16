#include "Platform.h"
#include "PlatformDetection.h"

std::string ModulePathCache;
std::string ModPathCache;
std::string ModRunPathCache;
std::string ModEdPathCache;
std::string BinPathCache;
std::string IntPathCache;
std::string ObjPathCache;
std::string LibPathCache;
std::string EngineResPathCache;

OSType GetHostOS()
{
#if defined(RBUILD_HOST_OSWINDOWS)
	return OSType::WINDOWS;
#elif defined(RBUILD_HOST_OS_LINUX)
	return OSType::LINUX;
#elif defined(RBUILD_HOST_OSX)
	return OSType::OSX;
#endif

	// Should never get here
	return OSType::WINDOWS;
}

ArchitectureType GetHostArchitecture()
{
	// Detect host architecture
#if defined(RBUILD_HOST_ARCH_X64)
	return ArchitectureType::X64;
#elif defined(RBUILD_HOST_ARCH_X86)
	return ArchitectureType::X86;
#elif defined(RBUILD_HOST_ARCH_ARM)
	return ArchitectureType::ARM;
#elif defined(RBUILD_HOST_ARCH_ARM64)
	return ArchitectureType::ARM64;
#endif

	// We should never get here
	return ArchitectureType::ARM64;
}

ToolsetType GetHostToolset()
{
#if defined(RBUILD_HOST_TOOLSET_GCC)
	return ToolsetType::GCC;
#elif defined(RBUILD_HOST_TOOLSET_MSVC)
	return ToolsetType::MSVC;
#elif defined(RBUILD_HOST_TOOLSET_Clang)
	return ToolsetType::Clang;
#endif

	// Should never get here
	return ToolsetType::CLANG;
}
