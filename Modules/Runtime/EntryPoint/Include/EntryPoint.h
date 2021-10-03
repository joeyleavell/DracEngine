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

	bool HasOption(Ry::ArrayList<Ry::String>& Args, Ry::String Option);
	Ry::StringView ParseOption(Ry::ArrayList<Ry::String>& Args, Ry::String Option);

	template<typename Class>
	int LaunchStandalone(int ArgC, char** ArgV, Ry::String Name)
	{
		Ry::ArrayList<Ry::String> Options;

		for (int32 Opt = 1; Opt < ArgC; Opt++)
		{
			Options.Add(ArgV[Opt]);
		}

		Ry::RenderingPlatform Plat = Ry::RenderingPlatform::Vulkan;

		if (Ry::HasOption(Options, "Vulkan"))
		{
			Plat = Ry::RenderingPlatform::Vulkan;
		}

		if (Ry::HasOption(Options, "OpenGL"))
		{
			Plat = Ry::RenderingPlatform::OpenGL;
		}

		Ry::Application* App = MakeApplication(Name, Plat, new Class);
		App->Run();

		return 0;
	}
	
#ifdef RYBUILD_DISTRIBUTE

#ifdef RYBUILD_TARGET_Windows

#include <windows.h>
/**
 * Main entry point definition for windows applications.
 */
#define IMPLEMENT_GAME(Class, Name) \
	int WinMain( \
		HINSTANCE hInstance, \
		HINSTANCE hPrevInstance, \
		LPSTR     lpCmdLine, \
		int       nShowCmd \
	) \
	{ \
		Ry::String CmdAsString = lpCmdLine; \
		Ry::StringView* ArgVView = nullptr; \
		int32 ArgC = CmdAsString.split(" ", &ArgVView); \
		char** ArgV = new char* [ArgC]; \
		for (int32 Arg = 0; Arg < ArgC; Arg++) \
		{ \
			ArgV[Arg] = new char[ArgVView[Arg].getSize()]; \
			Ry::StringCopy(ArgV[Arg], ArgVView->GetData(), ArgVView->getSize()); \
		}\
		delete[] ArgVView; \
		int32 Return = LaunchStandalone<Class>(ArgC, ArgV, #Name); \
		for (int32 Arg = 0; Arg < ArgC; Arg++) \
		{ \
			delete[] ArgV[Arg]; \
		}\
		delete[] ArgV; \
		return Return; \
	}
#else
	/**
	 * Main entry point definition for other OS applications.
	 */
	#define IMPLEMENT_GAME(Class, Name) \
	int main(int ArgC, char** ArgV) \
	{ \
		return LaunchStandalone<Class>(ArgC, ArgV, #Name); \
	}
#endif

#else

	#define IMPLEMENT_GAME(Class, Name) \
	extern "C" EXPORT_ONLY Ry::AbstractGame* CreateGame() \
	{ \
		return new (Class); \
	}

#endif


}