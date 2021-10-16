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

// ----------- Platform Specific Implementations ----------- 
std::string GetPlatformExecutableExt();
std::string GetModulePath();
bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize = 0, char* StdOut = nullptr, int ErrorBuffSize = 0, char* StdErr = nullptr);
