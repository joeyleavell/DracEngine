#pragma once

#include "Core/String.h"

namespace Ry
{
	
	CORE_MODULE void* GetPlatformModuleHandle();
	CORE_MODULE Ry::String GetPlatformModulePath();
	CORE_MODULE Ry::String GetPlatformModuleParentPath();
	
}