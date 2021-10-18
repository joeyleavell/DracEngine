

#include <iostream>
#include "File/File.h"
#include "Application.h"
#include "EntryPoint.h"
#include "Core/PlatformProcess.h"
#include "EditorMain.h"
#include "CommandUtils.h"
#include "Language/ShaderCompiler.h"
#include "Editor.h"
#include "Core/String.h"

typedef Ry::AbstractGame* (*CreateGameFunction)(void);

#ifdef RBUILD_TARGET_OS_LINUX
Ry::Application* LoadApplication(const Ry::String& SOLocation, Ry::RenderingPlatform Platform)
{
	return nullptr;
}
#endif

#ifdef RBUILD_TARGET_OS_WINDOWS
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
			std::cerr << "Game module not found in library " << *DllName << " " << std::endl;
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

Ry::String FindNonOption(Ry::ArrayList<Ry::String>& Options)
{
	for (const Ry::String& Opt : Options)
	{
		if (Opt.getSize() > 0 && Opt[0] != '-')
		{
			return Opt;
		}
	}

	return "";
}

void LaunchProject(Ry::ArrayList<Ry::String>& Options, Ry::RenderingPlatform Plat)
{

	Ry::String Path = FindNonOption(Options);

	// Run the loaded application
	Ry::String DllPath = Path;
	Ry::Application* App = LoadApplication(DllPath, Plat);

	if (App)
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

void CompileShaders(Ry::ArrayList<Ry::String>& Options)
{
	Ry::Log = new Ry::Logger;

	Ry::String OutputPath;

	if(Ry::HasOption(Options, "Output"))
	{
		OutputPath = Ry::ParseOption(Options, "Output");
	}

	// Mount shaders
	Ry::String EngineRoot = Ry::File::GetParentPath(Ry::File::GetParentPath(Ry::GetPlatformModulePath()));
	Ry::String ShadersRoot = Ry::File::Join(EngineRoot, "Shaders");
	Ry::File::MountDirectory(ShadersRoot, "Shaders");

	Ry::CompileAll(OutputPath);
}

int main(int ArgC, char** ArgV)
{
	// if(ArgC < 2)
	// {
	// 	std::cerr << "Must provide a command to the editor, project launcher not supported yet" << std::endl;
	// 	return 1;
	// }

	Ry::String Command = "";

	if(ArgC > 1 && ArgV[1][0] != '-')
	{
		Command = ArgV[1];
	}

	// Only add options after editor command
	Ry::ArrayList<Ry::String> Options;

	if(Command == "")
	{
		for (int32 Opt = 1; Opt < ArgC; Opt++)
		{
			Options.Add(ArgV[Opt]);
		}
	}
	else
	{
		for (int32 Opt = 2; Opt < ArgC; Opt++)
		{
			Options.Add(ArgV[Opt]);
		}
	}

	if(Command == "launch" || Command == "")
	{
		//"-RenderAPI=[OpenGL|GLES|Metal|Vulkan|DX12|DX11]"

		Ry::RenderingPlatform Plat = Ry::RenderingPlatform::Vulkan;

		if (Ry::HasOption(Options, "Vulkan"))
		{
			Plat = Ry::RenderingPlatform::Vulkan;
		}

		if (Ry::HasOption(Options, "OpenGL"))
		{
			Plat = Ry::RenderingPlatform::OpenGL;
		}


		if(Command == "launch")
		{
			LaunchProject(Options, Plat);
		}
		else
		{
			// Todo: project launcher
			Ry::Editor* Ed = new Ry::Editor(Plat);
			Ed->Run();
		}
	}
	else if(Command == "compile-shaders")
	{
		CompileShaders(Options);
	}
	
	return 0;
}