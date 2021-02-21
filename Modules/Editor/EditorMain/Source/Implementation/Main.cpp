

#include <iostream>
#include "File/File.h"
#include "Application.h"
#include "EntryPoint.h"
#include "Core/PlatformProcess.h"
#include "EditorMain.h"
#include "CommandUtils.h"

typedef Ry::AbstractGame* (*CreateGameFunction)(void);

#ifdef LINUX_IMPLEMENTATION
Ry::Application* LoadApplication(const Ry::String& DLLLocation)
{
	return nullptr;
}
#endif


#ifdef WINDOWS_IMPLEMENTATION
#include <windows.h>
Ry::Application* LoadApplication(const Ry::String& DLLLocation, Ry::RenderingPlatform Platform)
{
	Ry::AbstractGame* ResultGame;
	
	Ry::String DllPath = Ry::File::ConvertToAbsolute(DLLLocation);
	
	//Ry::String DllName = DllPath.filename().string().substr(0, DllPath.filename().string().size() - DllPath.extension().string().size()).c_str();
	Ry::String DllName = Ry::File::GetFileStem(DllPath);
	Ry::String DllContainingFolder = Ry::File::GetParentPath(DllPath);
	//
	// Ry::String DllContainingFolder = Filesystem::absolute(DllPath.parent_path()).string().c_str();

	std::cout << *DllName << std::endl;

#ifdef _WIN32

	// Load the game module DLL and attempt to find the module creation procedure

	SetDllDirectory(*DllContainingFolder);
	//std::cerr << "Could not load game module " << *DllContainingFolder << " returned error code " << GetLastError() << std::endl;

	const HINSTANCE LibraryInstance = LoadLibrary(*DllName);

	if (LibraryInstance)
	{
		const auto Function = GetProcAddress(LibraryInstance, "CreateGame");
		const auto Function2 = reinterpret_cast<CreateGameFunction>(Function);

		if (Function2)
		{
			ResultGame = Function2();
		}
		else
		{
			std::cerr << "Game module not found in library " << *DllName << " " << Function << std::endl;
			return nullptr;
		}
	}
	else
	{
		std::cerr << "Could not load game module " << *DllContainingFolder << " returned error code " << GetLastError() << std::endl;
		return nullptr;
	}
#endif

	if (ResultGame)
	{
		// TODO: Load this from project file
		return Ry::MakeApplication("Game", Platform, ResultGame);
	}
	else
	{
		std::cerr << "Game module creation failed." << std::endl;
		return nullptr;
	}
}
#endif

int main(int ArgC, char** ArgV)
{
	Ry::String NonOption = "";
	
	Ry::ArrayList<Ry::String> Options;
	for(int32 Opt = 0; Opt < ArgC; Opt++)
	{
		Options.Add(ArgV[Opt]);

		Ry::String OptStr = ArgV[Opt];

		if(OptStr.getSize() > 0 && OptStr[0] != '-')
		{
			NonOption = OptStr;
		}
	}

	Ry::RenderingPlatform Plat = Ry::RenderingPlatform::OpenGL;

	if (Ry::HasOption(Options, "Vulkan"))
	{
		Plat = Ry::RenderingPlatform::Vulkan;
	}

	//"-RenderAPI=[OpenGL|GLES|Metal|Vulkan|DX12|DX11]"

	// If there was a non option argument, assume it's the path of a game to startup with
	// Otherwise, startup project launcher
	if(NonOption.IsEmpty())
	{
		// Todo: this is where the project launcher should be started

		Ry::Editor* Ed = new Ry::Editor(Plat);
		Ed->Run();
	}
	else
	{
		// Run the loaded application
		Ry::String DllPath = NonOption;
		Ry::Application* App = LoadApplication(DllPath, Plat);

		if(App)
		{
			Ry::String DllParent = Ry::File::GetParentPath(DllPath);
			Ry::String ResourcesDir = Ry::File::GetParentPath(DllParent) + "\\Resources";
			Ry::String DataDir = Ry::File::GetParentPath(DllParent) + "\\Data";

			// Mount game resources and data here
			Ry::File::MountDirectory(Ry::File::ConvertToAbsolute(ResourcesDir), "Content");
			Ry::File::MountDirectory(Ry::File::ConvertToAbsolute(DataDir), "Data");

			App->Run();
		}
	}
	
	return 0;
}