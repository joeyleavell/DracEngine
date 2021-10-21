#include "Generate/ProjectFileGenerator.h"
#include "Common.h"
#include <string>
#include <iostream>
#include "Build/Module/Module.h"
#include "Util/Util.h"
#include "Generate/Windows/VisualStudioProjectFileGenerator.h"
#include "Generate/Xcode/XCodeProjectFileGenerator.h"

std::string GetDefaultPlatformCompiler()
{
#if defined(RBUILD_HOST_OS_WINDOWS)
	return "MSVC";
#elif defined(RBUILD_HOST_OS_LINUX)
	return "GCC;
#elif defined(RBUILD_HOST_OS_OSX)
	return "Clang";
#endif

	// Should never get here
	return "None";
}

bool CheckGeneratorSupported(std::string Generator)
{
	static std::set<std::string> Generators = {"VisualStudio2019", "Xcode"};

	return Generators.find(Generator) != Generators.end();
}

bool CheckCompilerSupported(std::string Compiler)
{
	std::set<std::string> CompilersSupported;
#if defined(RBUILD_HOST_OS_WINDOWS)
	CompilersSupported = { "MSVC", "GCC", "MinGW" }; // GCC and MinGW are the same thing here
#elif defined(RBUILD_HOST_OS_LINUX)
	CompilersSupported = { "GCC"};
#elif defined(RBUILD_HOST_OS_OSX)
	CompilersSupported = { "Clang" };
#endif

	return CompilersSupported.find(Compiler) != CompilersSupported.end();
}

std::string GetDefaultPlatformGenerator()
{
#if defined(RBUILD_HOST_OS_WINDOWS)
	return "VisualStudio2019";
#elif defined(RBUILD_HOST_OS_LINUX)
	return "Unix Makefiles";
#elif defined(RBUILD_HOST_OS_OSX)
	return "Xcode";
#endif

	// Should never get here
	return "None";
}

bool CallGeneratorImplementation_Engine(std::string Generator, std::string EnginePath, std::string Compiler)
{
	if (Generator == "VisualStudio2019")
		return GenerateEngineProjectFiles_VisualStudio(EnginePath, Compiler);
	if (Generator == "Xcode")
		return GenerateEngineProjectFiles_XCode(EnginePath, Compiler);

	// Should never get here, assume generator was already checked
	return false;
}

bool CallGeneratorImplementation_Game(std::string Generator, std::string GameName, std::string Path, std::string EngineRootPath, std::string Compiler)
{
	if (Generator == "VisualStudio2019")
		return GenerateGameProjectFiles_VisualStudio(GameName, Path, EngineRootPath, Compiler);
	if (Generator == "Xcode")
		return GenerateGameProjectFiles_XCode(GameName, Path, EngineRootPath, Compiler);

	// Should never get here, assume generator was already checked
	return false;
}

bool GenerateProjectFilesCmd(std::vector<std::string>& Args)
{
	// Expected command: Rybuild Generate ProjectFiles [-engine] [-game] <ProjectPath>
	// -Compiler=MSVC/GCC/Clang (mostly used for windows)
	// -Generator=VisualStudio2019, Xcode, Makefile, CLion, etc.

	if (Args.size() < 3)
	{
		IncorrectUsage("generate ProjectFiles [-Engine|-Game] [Options] <Path>");
		return false;
	}

	std::string Path = Args.back();

	bool bEngineOption = HasOption(Args, "-Engine");
	bool bGameOption = HasOption(Args, "-Game");

	std::string Compiler = GetDefaultPlatformCompiler();
	std::string Generator = GetDefaultPlatformGenerator();

	if (HasOption(Args, "Compiler"))
	{
		Compiler = ParseUniqueOption(Args, "Compiler");
	}

	if (HasOption(Args, "Generator"))
	{
		Generator = ParseUniqueOption(Args, "Generator");
	}

	// Check if the specified compiler was supported
	if(!CheckCompilerSupported(Compiler))
	{
		std::cerr << "Compiler " << Compiler << " not supported for platform " << OSToString(GetHostOS()) << std::endl;
		return false;
	}

	if(!CheckGeneratorSupported(Generator))
	{
		std::cerr << "Generator " << Generator << " not supported" << std::endl;
		return false;
	}

	if (bEngineOption && bGameOption)
	{
		std::cerr << "Specified both -Engine and -Game, must only specify one" << std::endl;
		return false;
	}
	else if (bEngineOption)
	{
		// Ensure path is specified
		if (Args.size() < 4)
		{
			IncorrectUsage("generate ProjectFiles -Engine [Options] <Path>");
			return false;
		}

		return CallGeneratorImplementation_Engine(Generator, Path, Compiler);
	}
	else if (bGameOption)
	{
		if (Args.size() < 5)
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

		return CallGeneratorImplementation_Game(Generator, GameName, Path, EngineRootPath, Compiler);
	}

	return false;
}