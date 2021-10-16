#pragma once

#include <vector>
#include <string>

enum class OSType
{
	/** Target the Microsoft Windows operating system */
	WINDOWS,

	/** Target the Linux operating system */
	LINUX,

	/** Target the MAC operating system */
	OSX
};

enum class ArchitectureType
{
	/** Target x86 (32bit) instruction set */
	X86,

	/** Target x86_64 (64bit) instruction set */
	X64,

	/** Target arm instruction set */
	ARM,

	/** Target arm64 instruction set */
	ARM64
};

enum class ToolsetType
{
	/** Microsoft visual studio compiler */
	MSVC,

	/** GNU compiler collection */
	GCC,

	/** Clang compiler suite based on LLVM */
	CLANG
};

extern std::string ModulePathCache;
extern std::string ModPathCache;
extern std::string ModRunPathCache;
extern std::string ModEdPathCache;
extern std::string BinPathCache;
extern std::string IntPathCache;
extern std::string ObjPathCache;
extern std::string LibPathCache;
extern std::string EngineResPathCache;

OSType GetHostOS();
ArchitectureType GetHostArchitecture();
ToolsetType GetHostToolset();

inline std::string OSToString(OSType Type)
{
	switch (Type)
	{
	case OSType::OSX:
		return "OSX";
	case OSType::WINDOWS:
		return "Windows";
	case OSType::LINUX:
		return "Linux";
	}
	return "None";
}

inline std::string ArchToString(ArchitectureType Type)
{
	switch (Type)
	{
	case ArchitectureType::X64:
		return "x64";
	case ArchitectureType::X86:
		return "x86";
	case ArchitectureType::ARM:
		return "Arm";
	case ArchitectureType::ARM64:
		return "Arm64";
	}
	return "None";
}

inline std::string ToolsetToString(OSType OS, ToolsetType Type)
{
	switch (Type)
	{
	case ToolsetType::CLANG:
		return "Clang";
	case ToolsetType::GCC:
		if(OS == OSType::WINDOWS)
			return "MinGW";
		else
			return "GCC";
	case ToolsetType::MSVC:
		return "MSVC";
	}
	return "None";
}

// ----------- Platform Specific Implementations ----------- 
std::string GetPlatformExecutableExt();
std::string GetModulePath();
bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize = 0, char* StdOut = nullptr, int ErrorBuffSize = 0, char* StdErr = nullptr);
