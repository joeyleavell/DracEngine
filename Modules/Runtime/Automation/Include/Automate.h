#pragma once

#include "Core/String.h"
#include "AutomationGen.h"

namespace Ry
{

	AUTOMATION_MODULE void ReloadResourceDirectory(const Ry::String& SourceResources, const Ry::String& DstResources);
	
}
