#include <iostream>
#include <string>
#include "Common.h"
#include "Util/Util.h"

int main(int ArgC, char** ArgV)
{
	if(ArgC < 2)
	{
		std::cerr << "Incorrect usage: <GameRoot> [Options]" << std::endl;
		return 1;
	}

	std::vector<std::string> CmdArgs;
	for(int CmdArg = 1; CmdArg < ArgC; CmdArg++)
	{
		CmdArgs.push_back(ArgV[CmdArg]);
	}

	std::string GameModuleRoot = Filesystem::canonical(ArgV[1]).string();
	std::string GameResources = (Filesystem::path(GameModuleRoot) / "Resources").string();
	std::string GameModules = (Filesystem::path(GameModuleRoot) / "Modules").string();
	std::string RyBuildRoot = Filesystem::path(GetModulePath()).parent_path().string();
	std::string BuildDir;

	std::string RyBuildModule = (Filesystem::path(RyBuildRoot) / ("RyBuild" + GetPlatformExecutableExt())).string();

	std::vector<std::string> BuildArgs;
	BuildArgs.push_back("rebuild");	
	BuildArgs.push_back(GameModules);
	BuildArgs.push_back("-BuildType=Standalone");
	BuildArgs.push_back("-Distribute");

	// Parse the toolchain
	if (HasOption(CmdArgs, "-Toolchain"))
	{
		std::string Toolchain = ParseOption(CmdArgs, "-Toolchain");

		BuildArgs.push_back("-" + Toolchain);
	}
	else
	{
		// Otherwise, pick it based on the OS
#ifdef RYBUILD_WINDOWS
		BuildArgs.push_back("-MSVC");
#elif defined(RYBUILD_LINUX) || defined(RYBUILD_MAC)
		BuildArgs.push_back("-GCC");
#endif
	}

	if (HasOption(CmdArgs, "-TargetOS"))
	{
		std::string TargetOS = ParseOption(CmdArgs, "-TargetOS");

		BuildArgs.push_back("-TargetOS=" + TargetOS);
	}

	if (HasOption(CmdArgs, "-TargetArch"))
	{
		std::string TargetArch = ParseOption(CmdArgs, "-TargetArch");

		BuildArgs.push_back("-TargetArch=" + TargetArch);
	}

	if (HasOption(CmdArgs, "-OutputDirectory"))
	{
		std::string Output = ParseOption(CmdArgs, "-OutputDirectory");

		BuildArgs.push_back("-OutputDirectory=" + Output);
		BuildDir = Filesystem::absolute(Output).string();
	}
	else
	{
		// Place the output in the Builds directory
		std::string GameBuildsRoot = (Filesystem::canonical(ArgV[1]) / "Builds").string();
		BuildDir = (Filesystem::path(GameBuildsRoot) / ("Build_" + GetTimestamp("%m_%e_%H_%M"))).string();
		BuildArgs.push_back("-OutputDirectory=" + BuildDir);
	}

	Filesystem::create_directories(BuildDir);

	if (HasOption(BuildArgs, "-BuildConfig"))
	{
		std::string BuildConfig = ParseOption(BuildArgs, "-BuildConfig");
		BuildArgs.push_back("-BuildConfig=" + BuildConfig);
	}
	else
	{
		BuildArgs.push_back("-BuildConfig=Shipping"); // Default to the shipping configuration (no debug info, high optimizations)
	}
	
	std::cout << "========== BUILDING ===============" << std::endl;
	std::cout << RyBuildModule << std::endl;
	for(std::string& S : BuildArgs)
	{
		std::cout << S << std::endl;
	}
	
	if(ExecProc(RyBuildModule, BuildArgs))
	{
		std::cout << "========== BUILD SUCCESS ==========" << std::endl;
	}
	else
	{
		std::cout << "========== BUILD FAILURE ==========" << std::endl;
		return 1;
	}

	std::cout << "========== PACKAGING ASSETS ==========" << std::endl;

	// Create resources directory in build directory
	Filesystem::create_directories(Filesystem::path(BuildDir) / "Resources");
	
	Filesystem::copy_options CopyOptions = Filesystem::copy_options::recursive | Filesystem::copy_options::overwrite_existing;

	// Copy over engine resources
	std::string EngineResDir = GetEngineResourcesDir();
	if(Filesystem::exists(EngineResDir))
	{
		Filesystem::copy(EngineResDir, Filesystem::path(BuildDir) / "Resources" / "Engine", CopyOptions);
	}
	else
	{
		std::cout << "No engine resources found!" << std::endl;
	}

	// Copy over game resources

	if(Filesystem::exists(GameResources))
	{
		Filesystem::copy(GameResources, Filesystem::path(BuildDir) / "Resources" / "Game", CopyOptions);
	}
	else
	{
		std::cout << "No game resources found!" << std::endl;
	}

	std::cout << "========== PACKAGING ASSETS COMPLETE ==========" << std::endl;

	bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize = 0, char* StdOut = nullptr, int ErrorBuffSize = 0, char* StdErr = nullptr);

	std::cout << "========== COMPILING SHADERS ==========" << std::endl;
	{
		std::string EngineExe = GetEngineExecutable();

		std::vector<std::string> ShaderCompCmdArgs;
		ShaderCompCmdArgs.push_back("compile-shaders");
		ShaderCompCmdArgs.push_back("-Output=" + BuildDir);

		if(!ExecProc(EngineExe, ShaderCompCmdArgs))
		{
			std::cout << "========== SHADER COMPILATION FAILED ==========" << std::endl;

			return 1;
		}
		else
		{
			std::cout << "========== SHADER COMPILATION SUCCEEDED ==========" << std::endl;
		}
	}

	return 0;
}