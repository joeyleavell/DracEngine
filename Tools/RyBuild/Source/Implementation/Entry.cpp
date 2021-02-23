#include <iostream>
#include <fstream>
#include "Build/Module/Module.h"
#include "Json/json.hpp"
#include "Common.h"
#include "Util/Util.h"
#include "Build/BuildTool.h"

#ifdef RYBUILD_WINDOWS
	#include "Generate/Windows/WindowsProject.h"
	#include "Generate/Windows/WindowsProjectBase.h"
	#include "Generate/Windows/WindowsProjectSolution.h"
#endif

enum OutputType
{
	EXE, DLL, LIB
};

bool GenerateEngineProjectFiles(std::string EngineRootPath, std::string Compiler)
{
#ifdef RYBUILD_WINDOWS

	std::string ProjectFilesDir = "Intermediate/ProjectFiles/";
	std::string EngineProjectName = "AryzeEngine";
	std::string WorkingDir = (Filesystem::canonical(EngineRootPath) / "Binary").string();
	std::string EngineBinary = (Filesystem::canonical(EngineRootPath) / "Binary" / "RyRuntime-EditorMain.exe").string();

	BuildSettings Settings;
	Settings.Toolset = BuildToolset::MSVC;
	Settings.TargetPlatform.OS = TargetOS::Windows;

	std::string EngineModulesDir = GetEngineModulesDir();
	
	// Need to discover modules for project generation
	std::vector<Module*> EngineModules;
	LoadModules(EngineModulesDir, EngineModules, &Settings);

	// Generate an engine project
	RyBuild::WindowsProjectSolution* Solution = new RyBuild::WindowsProjectSolution("AryzeEngine", EngineRootPath);
	RyBuild::WindowsProject* ModulesProject = new RyBuild::WindowsProject("Modules", EngineRootPath);

	// Set the discovered modules
	ModulesProject->SetModules(EngineModules);

	// Setup NMake batch commands for build, clean and rebuild
	Filesystem::path BuildScriptsFolder = Filesystem::canonical(EngineRootPath) / "BuildScripts" / "BatchFiles";
	ModulesProject->SetPhysicalName("AryzeEngine"); // This sets the name of the .vcxproj that gets generated.
	ModulesProject->SetArtifactDirectory((Filesystem::path(GetEngineIntermediateDir()) / "ProjectFiles").string());
	ModulesProject->SetBuildCmdLine("\"" + (BuildScriptsFolder / "Build.bat").string() + "\" " + EngineModulesDir + " -" + Compiler);
	ModulesProject->SetRebuildCmdLine("\"" + (BuildScriptsFolder / "Rebuild.bat").string() + "\" " + EngineModulesDir + " -" + Compiler);
	ModulesProject->SetCleanCmdLine("\"" + (BuildScriptsFolder / "Clean.bat").string() + "\" " + EngineModulesDir);
	ModulesProject->SetExecutable(EngineBinary);
	ModulesProject->SetWorkingDirectory(WorkingDir);

	Solution->AddSubProject(ModulesProject);
	//ModulesFolder->AddSubProject(RuntimeProjectFolder);
	//EngineFolder->AddSubProject(BuildProjectFolder);

	// Generate all required project files
	return Solution->GenerateProjects();

#else
	std::cerr << "Visual studio project generator not supported on this platform" << std::endl;
	return false;
#endif	
}

bool GenerateGameProjectFiles(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler)
{
#ifdef RYBUILD_WINDOWS

	std::string GameModulesDir = "$(SolutionDir)Modules";
	std::string EngineModulesDir = (Filesystem::canonical(Filesystem::path(EngineRootPath) / "Modules")).string();
	std::string WorkingDirectory = "$(SolutionDir)Binary";

	BuildSettings Settings;
	Settings.Toolset = BuildToolset::MSVC;
	Settings.TargetPlatform.OS = TargetOS::Windows;

	// Need to discover modules for project generation
	std::vector<Module*> EngineModules;
	LoadModules(EngineModulesDir, EngineModules, &Settings);

	std::vector<Module*> GameModules;
	LoadModules((Filesystem::path(GameRootPath) / "Modules").string(), GameModules, &Settings);

	// std::string ProjectFilesDir = (Filesystem::path(GameRootPath) / "Intermediate" / "ProjectFiles").string();
	std::string ProjectFilesDir = (Filesystem::path(GameRootPath) / "Intermediate" / "ProjectFiles").string();
	std::string EngineProjectName = "AryzeEngine";

	// Generate an engine project
	RyBuild::WindowsProjectSolution* GameSolution = new RyBuild::WindowsProjectSolution(GameName, GameRootPath);
	RyBuild::WindowsProject* EngineProject= new RyBuild::WindowsProject(EngineProjectName, GameRootPath);
	RyBuild::WindowsProject* GameProject = new RyBuild::WindowsProject("Modules", GameRootPath);

	// Set the discovered modules
	EngineProject->SetModules(EngineModules);

	GameProject->SetModules(GameModules);
	GameProject->SetIncludeModules(EngineModules); /* Allow game project intellisense search access to all engine modules */

	std::string EngineBinary = (Filesystem::path(EngineRootPath) / "Binary" / "RyRuntime-EditorMain.exe").string();
	std::string GameDLL = "" + (Filesystem::path(GameRootPath) / "Binary" / "RyRuntime-").string() + GameName + ".dll";
	//EngineBinary += GameDLL; // Give the engine main the path to the game DLL
	
	// Setup NMake batch commands for build, clean and rebuild
	Filesystem::path BuildScriptsFolder = Filesystem::canonical(EngineRootPath) / "BuildScripts" / "BatchFiles";

	GameProject->SetPhysicalName(GameName);
	GameProject->SetBuildCmdLine("\"" + (BuildScriptsFolder / "Build.bat").string() + "\" " + GameModulesDir + " -" + Compiler);
	GameProject->SetRebuildCmdLine("\"" + (BuildScriptsFolder / "Rebuild.bat").string() + "\" " + GameModulesDir + " -" + Compiler);
	GameProject->SetCleanCmdLine("\"" + (BuildScriptsFolder / "Clean.bat").string() + "\" " + GameModulesDir);
	GameProject->SetModuleRoot(GameRootPath);
	GameProject->SetArtifactDirectory(ProjectFilesDir);
	GameProject->SetExecutable(EngineBinary);
	GameProject->SetArguments("launch " + GameDLL);
	GameProject->SetWorkingDirectory(WorkingDirectory);

	EngineProject->SetBuildCmdLine("\"" + (BuildScriptsFolder / "Build.bat").string() + "\" " + EngineModulesDir + " -" + Compiler);
	EngineProject->SetRebuildCmdLine("\"" + (BuildScriptsFolder / "Rebuild.bat").string() + "\" " + EngineModulesDir + " -" + Compiler);
	EngineProject->SetCleanCmdLine("\"" + (BuildScriptsFolder / "Clean.bat").string() + "\" " + EngineModulesDir);
	EngineProject->SetModuleRoot(EngineRootPath);
	EngineProject->SetArtifactDirectory(ProjectFilesDir);
	EngineProject->SetExecutable(EngineBinary);
	EngineProject->SetArguments("launch " + GameDLL);
	EngineProject->SetWorkingDirectory(WorkingDirectory);

	GameSolution->AddSubProject(EngineProject);
	GameSolution->AddSubProject(GameProject);

	//ModulesFolder->AddSubProject(RuntimeProjectFolder);
	//EngineFolder->AddSubProject(BuildProjectFolder);

	// Generate all required project files
	return GameSolution->GenerateProjects();

#else
	std::cerr << "Visual studio project generator not supported on this platform" << std::endl;
	return false;
#endif
}

bool GenerateProjectFilesCmd(std::vector<std::string>& Args)
{
	// Rybuild Generate ProjectFiles [-engine] [-game] <ProjectPath>
	// Assume path is very last argument

	if(Args.size() < 3)
	{
		IncorrectUsage("generate ProjectFiles [-Engine|-Game] [Options] <Path>");
		return false;
	}

	std::string Path = Args.back();

	bool bEngineOption = HasOption(Args, "-Engine");
	bool bGameOption = HasOption(Args, "-Game");

	std::string Compiler = "MSVC";

	if (HasOption(Args, "MinGW"))
	{
		Compiler = "MinGW";
	}

	if(bEngineOption && bGameOption)
	{
		std::cerr << "Specified both -Engine and -Game, must only specify one" << std::endl;
		return false;
	}
	else if(bEngineOption)
	{
		// Ensure path is specified
		if (Args.size() < 4)
		{
			IncorrectUsage("generate ProjectFiles -Engine [Options] <Path>");
			return false;
		}

		return GenerateEngineProjectFiles(Path, Compiler);
	}
	else if(bGameOption)
	{
		if(Args.size() < 5)
		{
			IncorrectUsage("generate ProjectFiles [options] <GameName> <GameRoot>");
			return false;
		}

		// Game name is assumed to be second to last argument
		std::string GameName = Args[Args.size() - 2];

		// Make the fair assumption that we're in the ry build executable module
		std::string ThisExePath = GetModulePath();

		// Work our way back to the engine root
		std::string EngineRootPath = GetEngineRootDir();

		return GenerateGameProjectFiles(GameName, Path, EngineRootPath, Compiler);

	}

}

void GenerateModuleCmd(std::vector<std::string>& Args)
{
	// Rybuild Generate Module <ModuleRoot> <ModuleName>

	// Guard against incorrect commands
	if(Args.size() < 5)
	{
		IncorrectUsage("generate Module <ModuleType> <ModuleName> <ModuleRoot>");
		return;
	}

	std::string ModuleType = Args[2];
	std::string ModuleName = Args[3];
	std::string ModuleRoot = Args[4];

	if(ModuleType != "Game" && ModuleType != "Runtime" && ModuleType != "Executable")
	{
		std::cerr << "Invalid module type! Can be one of: Game, Runtime, or Program." << std::endl;
		return;
	}

	Filesystem::path ModulePath = Filesystem::path(ModuleRoot) / ModuleName;

	// Check for already existing module (error)
	if(Filesystem::exists(ModulePath))
	{
		std::cerr << "Module path already exists: " << Filesystem::canonical(ModulePath).string() << std::endl;
		return;
	}

	// Declare folders for module
	Filesystem::path ModuleFilePath = ModulePath / (ModuleName + ".build.py");

	Filesystem::create_directories(ModulePath / "Source");
	Filesystem::create_directories(ModulePath / "Source" / "Include");
	Filesystem::create_directories(ModulePath / "Source" / "Implementation");

	// Write out module file here so it's generated

	// Json ModuleJson;
	// ModuleJson["Name"] = ModuleName;
	//
	// if(ModuleType == "Game" || ModuleType == "Executable")
	// {
	// 	ModuleJson["Type"] = "Executable";
	// }
	// else
	// {
	// 	ModuleJson["Type"] = "Runtime";
	// }
	//
	// ModuleJson["Modules"] = {"Core", "Application"};

	std::ofstream OutModuleFile;
	OutModuleFile.open(ModuleFilePath.string());
	{
		OutModuleFile << R"(Modules = ["Core", "Application", "EntryPoint"])" << std::endl;

		if(ModuleType == "Game" || ModuleType == "Executable")
		{
			OutModuleFile << R"(Type = "Executable")" << std::endl;
		}
		else
		{
			OutModuleFile << R"(Type = "Runtime")" << std::endl;
		}
	}
	OutModuleFile.close();

	// Generate starter game source files
	if(ModuleType == "Game")
	{
		std::ofstream OutGameHeader;

		OutGameHeader.open((Filesystem::path(ModulePath) / "Source" / "Include" / "Game.h").string());
		{
			OutGameHeader << R"(#pragma once

#include "Application.h"

using namespace Ry;

class Game : public Ry::AbstractGame
{

public:

	void Init() override;
	void Update(float Delta) override;
	void Render() override;
	void Quit() override;
};
)";
		}
		OutGameHeader.close();

		std::ofstream OutGameSource;
		OutGameSource.open((Filesystem::path(ModulePath) / "Source" / "Implementation" / "Game.cpp").string());
		{
			OutGameSource << R"(#include "Game.h"
#include "EntryPoint.h"

void Game::Init()
{
	// Use this function to initialize your assets
}

void Game::Update(float Delta)
{
	// Use this function to handle any per-tick logic such as AI, physics, etc.
}

void Game::Render()
{
	// Draw objects to the screen
}

void Game::Quit()
{
	// Called when the game is shutting down
}

IMPLEMENT_GAME(Game, "TestGame");
)";
		}
		OutGameSource.close();
		
	}
}

void GenerateGameCmd(std::vector<std::string>& Args)
{
	if (Args.size() < 4)
	{
		IncorrectUsage("generate Game <GameName> <GameRoot>");
		return;
	}

	// Name of the game
	std::string GameName = Args[2];

	// Path of the game
	std::string GamePath = (Filesystem::path(Args[3]) / GameName).string();
	std::string GameModulePath = (Filesystem::path(GamePath) / "Modules").string();
	std::string GameAssetsPath = (Filesystem::path(GamePath) / "Resources").string();

	// Create a game module
	std::vector<std::string> CreateModuleArgs = { "generate", "Module", "Game", GameName, GameModulePath };
	GenerateModuleCmd(CreateModuleArgs);

	Filesystem::create_directories(Filesystem::path(GamePath) / "BuildScripts" / "BatchFiles");

	// Create a courtesy project generation batch files
	{
		std::string GenerateProjectFilesBatLoc = (Filesystem::path(GamePath) / "BuildScripts" / "BatchFiles" / "GenerateGameFiles-VS-MSVC.bat").string();
		std::ofstream GenerateProjectFilesBatOut(GenerateProjectFilesBatLoc);
		{
			std::string RyBuildPath = GetModulePath();
			GenerateProjectFilesBatOut << "\"" << RyBuildPath << "\" generate ProjectFiles -Game -MSVC " << GameName << " \"" << Filesystem::canonical(GamePath) << "\"" << std::endl;
		}
		GenerateProjectFilesBatOut.close();
	}

	{
		std::string GenerateProjectFilesBatLoc = (Filesystem::path(GamePath) / "BuildScripts" / "BatchFiles" / "GenerateGameFiles-VS-MinGW.bat").string();
		std::ofstream GenerateProjectFilesBatOut(GenerateProjectFilesBatLoc);
		{
			std::string RyBuildPath = GetModulePath();
			GenerateProjectFilesBatOut << "\"" << RyBuildPath << "\" generate ProjectFiles -Game -MinGW " << GameName << " \"" << Filesystem::canonical(GamePath) << "\"" << std::endl;
		}
		GenerateProjectFilesBatOut.close();
	}

	// Create distribute batch file
	{
		std::string DistributeGameBatchLoc = (Filesystem::path(GamePath) / "BuildScripts" / "BatchFiles" / "Distribute-x64-Windows-MSVC.bat").string();
		std::ofstream DistributeGameBatchOut(DistributeGameBatchLoc);
		{
			std::string RyDistPath = (Filesystem::path(GetModulePath()).parent_path() / "RyDist.exe").string();
			DistributeGameBatchOut << "\"" << RyDistPath << "\" \"" << Filesystem::canonical(GamePath) << "\"" << " -Toolset=MSVC -TargetOS=Windows -TargetArch=x86_64 -BuildType=Standalone -Distribute" << std::endl;
		}
		DistributeGameBatchOut.close();
	}

	// Create assets directory
	Filesystem::create_directories(GameAssetsPath);

	// Create game project file?
}

void GenerateCmd(std::vector<std::string>& Args)
{

	if(Args.size() < 2)
	{
		IncorrectUsage("generate <Cmd> ...");
		return;
	}

	std::string GenerateCmd = Args[1];

	if(GenerateCmd == "ProjectFiles")
	{
		GenerateProjectFilesCmd(Args);
	}
	else if(GenerateCmd == "Module")
	{
		GenerateModuleCmd(Args);
	}
	else if(GenerateCmd == "Game")
	{
		GenerateGameCmd(Args);
	}
	else
	{
		std::cerr << "Generate command not recognized. Must be one of ProjectFiles, Module, or Game." << std::endl;
	}
	
}

int main(int ArgCount, char** ArgValues)
{
	
	if (ArgCount < 2)
	{
		std::cerr << "RyBuild requires you specify a command: generate, build" << std::endl;
		return 1;
	}

	std::vector<std::string> Args;

	std::string SubprogramCmd = ArgValues[1];
	for(int CmdIndex = 1; CmdIndex < ArgCount; CmdIndex++)
	{
		std::string Arg = ArgValues[CmdIndex];

		// Check if arg is verbose. This argument is pretty common, so store it globally.
		if(Arg.find("verbose") != std::string::npos)
		{
			bIsVerbose = true;
		}
		
		// Copy over arguments not including first
		Args.push_back(Arg);
	}

	if(SubprogramCmd == "generate")
	{
		GenerateCmd(Args);
	}
	else if(SubprogramCmd == "build")
	{
		if(!BuildCmd(Args))
		{
			return 1;
		}
	}
	else if (SubprogramCmd == "rebuild")
	{
		if(!RebuildCmd(Args))
		{
			return 1;
		}
	}
	else if(SubprogramCmd == "clean")
	{
		if(!CleanCmd(Args))
		{
			return 1;
		}
	}
	else
	{
		std::cerr << "Command not recognized" << std::endl;
	}

	return 0;
}