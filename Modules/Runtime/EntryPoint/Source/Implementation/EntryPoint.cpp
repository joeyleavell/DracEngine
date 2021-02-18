#include "EntryPoint.h"
#include "DesktopApplication.h"
#include "Application.h"

namespace Ry
{

	Ry::Application* MakeApplication(const Ry::String& AppName, Ry::AbstractGame* Game)
	{
		// Create application based on defined target
#if defined(RYBUILD_TARGET_WINDOWS) || defined(RYBUILD_TARGET_MAC) || defined(RYBUILD_TARGET_LINUX)
		return new Ry::DesktopApp(Game, AppName);
#else
		return nullptr;
#endif
	}

}
