#pragma once

#include "EntryPointGen.h"
#include "Core/Core.h"

namespace Ry
{
	enum class RenderingPlatform;
	class String;
	class Application;
	class AbstractGame;
	

	ENTRYPOINT_MODULE Ry::Application* MakeApplication(const Ry::String& AppName, Ry::RenderingPlatform Platform, Ry::AbstractGame* Game);

#ifdef RYBUILD_DISTRIBUTE
		/**
		 * Main entry point definition for windows applications.
		 */
	#define IMPLEMENT_GAME(Class, Name) \
	int main() \
	{ \
	\
		Ry::Application* App = MakeApplication(Name, Ry::RenderingPlatform::OpenGL, new (Class)); \
		App->Run(); \
	\
		return 0; \
	}

#else

	#define IMPLEMENT_GAME(Class, Name) \
	extern "C" EXPORT_ONLY Ry::AbstractGame* CreateGame() \
	{ \
		return new (Class); \
	}

#endif


}
