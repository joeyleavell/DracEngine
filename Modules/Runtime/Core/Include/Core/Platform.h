#pragma once

#include "Core/String.h"
#include "CoreGen.h"

// TODO: Add type size asserts here

namespace Ry
{
	/**
	 * All supported rendering platforms.
	 */
	enum class RenderingPlatform
	{
		OpenGL,
		Vulkan
	};

	CORE_MODULE void* GetPlatformModuleHandle();

	CORE_MODULE Ry::String GetPlatformModulePath();
	CORE_MODULE Ry::String GetPlatformModuleParentPath();

	CORE_MODULE void SetClipboardString(const Ry::String& Value);
	CORE_MODULE Ry::String GetClipboardString();

	CORE_MODULE double GetSystemTime();

}