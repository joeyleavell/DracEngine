#include "Core/Globals.h"
#include "File/File.h"
#include "Application.h"
#include "DesktopApplication.h"

namespace Ry
{
	// Default application creation

	#ifdef _WIN32
	#include <windows.h>
	#endif

	extern "C"
	{
		Ry::Application* MakeApplication(const Ry::String& app_name, Ry::AbstractGame* game)
		{
			return new Ry::DesktopApp(game, app_name);
		}
	}

}
