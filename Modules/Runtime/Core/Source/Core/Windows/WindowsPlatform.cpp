#ifdef RBUILD_TARGET_OS_WINDOWS

#include "Core/Windows/WindowsPlatform.h"
#include "Core/PlatformProcess.h"
#include "File/File.h"

// Windows include
#include "windows.h"

namespace Ry
{
	static HINSTANCE DllModuleHandle;

	void* GetPlatformModuleHandle()
	{
		return DllModuleHandle;
	}

	String GetPlatformModulePath()
	{
		CHAR Path[MAX_PATH];
		GetModuleFileNameA(DllModuleHandle, Path, MAX_PATH);

		return Ry::File::ConvertToAbsolute(Path);
	}

	String GetPlatformModuleParentPath()
	{
		return Ry::File::GetParentPath(GetPlatformModulePath());
	}

}

/**
 * The main entry point for the engine DLL.
 */
bool DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	Ry::DllModuleHandle = hinstDLL;

	return true;
}

#endif