#include "Application.h"
#include "EntryPoint.h"
#include "DesktopApplication.h"

namespace Ry
{

	bool HasOption(Ry::ArrayList<Ry::String>& Args, Ry::String Option)
	{
		for (Ry::String& Opt : Args)
		{
			if (Opt.find_first(Option, 0) >= 0)
			{
				return true;
			}
		}

		return false;
	}

	Ry::StringView ParseOption(Ry::ArrayList<Ry::String>& Args, Ry::String Option)
	{
		for (Ry::String& Opt : Args)
		{
			if (Opt.find_first(Option, 0) >= 0)
			{
				int SepIndex = Opt.find_first("=", 0);

				return Opt.substring(SepIndex + 1);
			}
		}

		return StringView(nullptr, 0, 0, 0);
	}

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
