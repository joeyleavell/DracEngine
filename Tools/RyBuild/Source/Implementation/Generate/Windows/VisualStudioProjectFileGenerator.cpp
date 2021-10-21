#include "Generate/Windows/VisualStudioProjectFileGenerator.h"
#include <string>
#include "Common.h"
#include "Build/BuildSettings.h"
#include "Util/Util.h"
#include "Build/Module/Module.h"
#include "Generate/Windows/WindowsProjectSolution.h"
#include "Generate/Windows/WindowsProject.h"

bool GenerateEngineProjectFiles_VisualStudio(std::string EngineRootPath, std::string Compiler)
{
#ifdef RBUILD_HOST_OS_WINDOWS

	std::string ProjectFilesDir = "Intermediate/ProjectFiles/";
	std::string EngineProjectName = "AryzeEngine";
	std::string WorkingDir = (Filesystem::canonical(EngineRootPath) / "Binary").string();
	std::string EngineBinary = (Filesystem::canonical(EngineRootPath) / "Binary" / "RyRuntime-EditorMain.exe").string();

	BuildSettings Settings;
	Settings.Toolset = ToolsetType::MSVC;
	Settings.TargetPlatform.OS = OSType::WINDOWS;

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
	ModulesProject->SetPhysicalName("AryzeEngine"); // This sets the name of the .vcxproj that gets Gamd.
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

bool GenerateGameProjectFiles_VisualStudio(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler)
{
#ifdef RBUILD_HOST_OS_WINDOWS

	std::string GameModulesDir = "$(SolutionDir)Modules";
	std::string EngineModulesDir = (Filesystem::canonical(Filesystem::path(EngineRootPath) / "Modules")).string();
	std::string WorkingDirectory = "$(SolutionDir)Binary";

	BuildSettings Settings;
	Settings.Toolset = ToolsetType::MSVC;
	Settings.TargetPlatform.OS = OSType::WINDOWS;

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
	RyBuild::WindowsProject* EngineProject = new RyBuild::WindowsProject(EngineProjectName, GameRootPath);
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