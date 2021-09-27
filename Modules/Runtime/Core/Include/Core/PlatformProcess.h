#pragma once

#include "Core/String.h"

namespace Ry
{
	
	CORE_MODULE void* GetPlatformModuleHandle();
	CORE_MODULE Ry::String GetPlatformModulePath();
	CORE_MODULE Ry::String GetPlatformModuleParentPath();

	CORE_MODULE void SetClipboardString(const Ry::String& Value);
	CORE_MODULE Ry::String GetClipboardString();

	CORE_MODULE double GetSystemTime();

}