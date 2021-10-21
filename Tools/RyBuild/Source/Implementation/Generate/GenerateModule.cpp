#include "Generate/GenerateModule.h"
#include <string>
#include "Util/Util.h"
#include <iostream>
#include <fstream>

void GenerateModuleCmd(std::vector<std::string>& Args)
{
	// Rybuild Generate Module <ModuleRoot> <ModuleName>

	// Guard against incorrect commands
	if (Args.size() < 5)
	{
		IncorrectUsage("generate Module <ModuleType> <ModuleName> <ModuleRoot>");
		return;
	}

	std::string ModuleType = Args[2];
	std::string ModuleName = Args[3];
	std::string ModuleRoot = Args[4];

	if (ModuleType != "Game" && ModuleType != "Runtime" && ModuleType != "Executable")
	{
		std::cerr << "Invalid module type! Can be one of: Game, Runtime, or Program." << std::endl;
		return;
	}

	Filesystem::path ModulePath = Filesystem::path(ModuleRoot) / ModuleName;

	// Check for already existing module (error)
	if (Filesystem::exists(ModulePath))
	{
		std::cerr << "Module path already exists: " << Filesystem::canonical(ModulePath).string() << std::endl;
		return;
	}

	// Declare folders for module
	Filesystem::path ModuleFilePath = ModulePath / (ModuleName + ".build.py");

	Filesystem::create_directories(ModulePath / "Include");
	Filesystem::create_directories(ModulePath / "Source");

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

		if (ModuleType == "Game" || ModuleType == "Executable")
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
	if (ModuleType == "Game")
	{
		std::ofstream OutGameHeader;

		OutGameHeader.open((Filesystem::path(ModulePath) / "Include" / "Game.h").string());
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
		OutGameSource.open((Filesystem::path(ModulePath) / "Source" / "Game.cpp").string());
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
