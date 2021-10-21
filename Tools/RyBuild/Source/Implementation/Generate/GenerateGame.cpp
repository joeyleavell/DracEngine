#include "Generate/GenerateGame.h"
#include "Generate/GenerateModule.h"
#include <vector>
#include <fstream>
#include "Util/Util.h"

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
