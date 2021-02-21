#include "EntryPoint.h"
#include "DesktopApplication.h"
#include "Application.h"

namespace Ry
{

	Ry::Application* MakeApplication(const Ry::String& AppName, Ry::RenderingPlatform Platform, Ry::AbstractGame* Game)
	{
		// Create application based on defined target
#if defined(RYBUILD_TARGET_Windows) || defined(RYBUILD_TARGET_Mac) || defined(RYBUILD_TARGET_Linux)
		return new Ry::DesktopApp(Game, Platform, AppName);
#else
		return nullptr;
#endif
	}

}
