#include "Common.h"

#ifdef RBUILD_HOST_OS_WINDOWS
#include <windows.h>
#include "Build/MSVCBuildTool.h"
#include "Build/Module/Module.h"
#include <map>
#include "Util/Util.h"
#include <iostream>
#include <fstream>

std::string GetWindowsSDKBaseDir()
{
	HKEY SdkInfoKey;
	char SdkDir[1024];
	DWORD Size = 1024;

	ZeroMemory(SdkDir, sizeof(SdkDir));

	LSTATUS CodeOpen = RegOpenKeyA(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0", &SdkInfoKey);
	LSTATUS CodeOne = RegQueryValueExA(SdkInfoKey, "InstallationFolder", NULL, NULL, (LPBYTE)SdkDir, &Size);

	return SdkDir;
}

std::string GetWindowsSDKVersion()
{
	HKEY SdkInfoKey;
	char SdkVersion[1024];
	DWORD Size = 1024;

	ZeroMemory(SdkVersion, sizeof(SdkVersion));

	LSTATUS CodeOpen = RegOpenKeyA(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0", &SdkInfoKey);
	LSTATUS CodeTwo = RegQueryValueExA(SdkInfoKey, "ProductVersion", NULL, NULL, (LPBYTE)SdkVersion, &Size);

	return SdkVersion;
}

std::string GetDefaultWindowsIncludePath(std::string SdkDir, std::string SdkVersion)
{

	Filesystem::path BaseIncludeDir = Filesystem::path(SdkDir) / "Include" / (std::string(SdkVersion) + ".0");

	Filesystem::path WinRTInclude = BaseIncludeDir / "winrt";
	Filesystem::path CPPWinRTInclude = BaseIncludeDir / "cppwinrt";
	Filesystem::path SharedInclude = BaseIncludeDir / "shared";
	Filesystem::path UCRTInclude = BaseIncludeDir / "ucrt";
	Filesystem::path UMInclude = BaseIncludeDir / "um";

	std::string ResultInclude;

	// Append to include paths
	ResultInclude = WinRTInclude.string();
	ResultInclude += ";" + CPPWinRTInclude.string();
	ResultInclude += ";" + SharedInclude.string();
	ResultInclude += ";" + UCRTInclude.string();
	ResultInclude += ";" + UMInclude.string();

	return ResultInclude;
}

std::string GetDefaultWindowsLibPath(std::string SdkDir, std::string SdkVersion)
{
	Filesystem::path BaseLibsDir = Filesystem::path(SdkDir) / "Lib" / (std::string(SdkVersion) + ".0");

	Filesystem::path UMLibs = BaseLibsDir / "um" / "x64";
	Filesystem::path UCRTLibs = BaseLibsDir / "ucrt" / "x64";

	// Append to library paths
	std::string LibsPath = UMLibs.string();
	LibsPath += ";" + UCRTLibs.string();

	return LibsPath;
}

std::string VsWhereQuery(std::string Query)
{
	std::string VSWherePath = R"("C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe")";
	std::vector<std::string> CmdArgs{ Query };

	constexpr int StdOutSize = 1024 * 100;
	char StdOut[StdOutSize];

	bool bResult = ExecProc(VSWherePath, CmdArgs, StdOutSize, StdOut, StdOutSize);

	if (bResult)
	{
		return StdOut;
	}
	else
	{
		std::cerr << "Failed to run vswhere " << StdOut << std::endl;
		return "";
	}
}

/**
 * Returns the first line in the VS query and caches it for later since vswhere is slow af.
 */
std::string VsWhereCached(std::string Query, std::string CacheFile)
{
	// Check if the location is cached in working directory
	std::string CLCachePath = std::experimental::filesystem::absolute(CacheFile).string();

	std::string Path;
	bool bNeedsCache = false;

	if (std::experimental::filesystem::exists(CLCachePath))
	{
		std::ifstream CacheIn(CLCachePath);
		std::string NextLine;

		while (std::getline(CacheIn, NextLine))
		{
			if (!NextLine.empty())
			{
				Path += NextLine + ";";
			}
		}
		CacheIn.close();
	}
	else
	{
		// Cache the result
		Path = VsWhereQuery(Query);
		bNeedsCache = true;
	}

	// Delimit on newline
	int NewLine = Path.find_first_of('\r');

	if (bNeedsCache)
	{
		std::ofstream CacheOut(CLCachePath);
		CacheOut << Path;
		CacheOut.close();
	}

	return Path;
}

std::string FindCLExe()
{
	std::string Query = R"(-latest -find "**\Hostx64\x64\cl.exe")";
	std::string CLExe = VsWhereCached(Query, "./CLExe.txt");

	// Delimit on newline
	int NewLine = CLExe.find_first_of('\r');

	if (NewLine == std::string::npos)
	{
		return CLExe;
	}
	else
	{
		return CLExe.substr(0, NewLine);
	}
}

std::string FindLinkExe()
{
	std::string Query = R"(-latest -find "**\Hostx64\x64\link.exe")";
	std::string LinkExe = VsWhereCached(Query, "./LinkExe.txt");

	// Delimit on newline
	int NewLine = LinkExe.find_first_of('\r');

	if (NewLine == std::string::npos)
	{
		return LinkExe;
	}
	else
	{
		return LinkExe.substr(0, NewLine);
	}
}

std::string FindSystemIncludePaths()
{
	std::string Query = R"(-latest -find "**\MSVC\**\include")";
	std::string Result = VsWhereCached(Query, "./SystemIncludePaths.txt");

	constexpr int Size = 1024 * 5;
	char ProcResult[Size];
	int ProcResultIndex = 0;

	for (int CharIndex = 0; CharIndex < Result.size(); CharIndex++, ProcResultIndex++)
	{
		if (Result[CharIndex] == '\r')
		{
			ProcResult[ProcResultIndex] = ';';
			CharIndex++; // Next index will be \n
		}
		else
		{
			ProcResult[ProcResultIndex] = Result[CharIndex];
		}
	}

	ProcResult[Result.size()] = 0;

	return ProcResult;
}

std::string FindSystemLibPaths()
{
	std::string Query = R"(-latest -find **\MSVC\**\lib\x64)";
	std::string Result = VsWhereCached(Query, "./SystemLibPaths.txt");
	
	constexpr int Size = 1024 * 5;
	char ProcResult[Size];
	int ProcResultIndex = 0;

	for (int CharIndex = 0; CharIndex < Result.size(); CharIndex++, ProcResultIndex++)
	{
		if (Result[CharIndex] == '\r')
		{
			ProcResult[ProcResultIndex] = ';';
			CharIndex++; // Next index will be \n
		}
		else
		{
			ProcResult[ProcResultIndex] = Result[CharIndex];
		}
	}

	ProcResult[Result.size()] = 0;

	return ProcResult;
}

MSVCBuildTool::MSVCBuildTool(std::string RootDir, BuildSettings Settings):
AbstractBuildTool(RootDir, Settings)
{
	this->ObjectFileExtension = ".obj";

	// Make the build environment for the tool
	std::string WindowsSdkBase = GetWindowsSDKBaseDir();
	std::string WindowsSdkVersion = GetWindowsSDKVersion();

	CLPath = FindCLExe();
	LinkPath = FindLinkExe();
	SystemIncludePath = FindSystemIncludePaths() + ";" + GetDefaultWindowsIncludePath(WindowsSdkBase, WindowsSdkVersion);
	SystemLibsPath = FindSystemLibPaths() + ";" + GetDefaultWindowsLibPath(WindowsSdkBase, WindowsSdkVersion);

	// Preset the environment variables for the compiler calls
	SetEnvironmentVariableA("INCLUDE", SystemIncludePath.c_str());
	SetEnvironmentVariableA("LIB", SystemLibsPath.c_str());
}

bool MSVCBuildTool::BuildSingleSource(const Module& TheModule, std::string OutputDirectory, std::string SourceFile, std::string& StdOut, std::string& StdErr)
{
	if(Settings.TargetPlatform.Arch != ArchitectureType::X64 && Settings.TargetPlatform.OS != OSType::WINDOWS)
	{
		std::cerr << "Only x86_64 windows targets are supported with the MSVC toolchain" << std::endl;
		return false;
	}
	
	std::string Program = CLPath;
	std::vector<std::string> BuildCmd;

	// Enable exception handling
	BuildCmd.push_back("/EHsc");

	// Enable all warnings
	BuildCmd.push_back("/W2");

	// Specify c++ 17 standard
	BuildCmd.push_back("/std:c++17");

	// Specify compile only
	BuildCmd.push_back("/c");
	
	// Setup the intermediate directory for output of the OBJ
	BuildCmd.push_back("/Fo\"" + OutputDirectory + "\\\\\"");

	// Use MD runtime
	BuildCmd.push_back("/MD");

	// Debug information
	if(Settings.Config == BuildConfiguration::DEVELOPMENT)
	{
		BuildCmd.push_back("/Z7");
	}

	BuildCmd.push_back("/DRYBUILD_TARGET_" + OSToString(Settings.TargetPlatform.OS));
	BuildCmd.push_back("/DRYBUILD_CONFIG_" + Settings.ConfigToString());
	
	if (Settings.Type == BuildType::STANDALONE)
	{
		BuildCmd.push_back("/DRYBUILD_STANDALONE");
	}

	if (Settings.bDistribute)
	{
		BuildCmd.push_back("/DRYBUILD_DISTRIBUTE");
	}

	// Define any preprocessor symbols here
	{
		// Register any extra macro definition requests by the module
		for (const std::string& MacroDef : TheModule.MacroDefinitions)
		{
			BuildCmd.push_back("/D" + MacroDef);
		}

		// Specify the compile module macro for DLL visibility. This macro is generated in the code generation phase.
		BuildCmd.push_back("/DCOMPILE_MODULE_" + ToUpper(TheModule.Name));
	}

	// Add the module's include path
	BuildCmd.push_back("/I\"" + TheModule.GetIncludeDir() + "\\\"");

	// Add the module's generated path
	BuildCmd.push_back("/I\"" + TheModule.GetGeneratedDir() + "\\\"");

	// Add module dependencies to include path
	for (const std::string& Dependency : TheModule.ModuleDependencies)
	{
		Module* Mod = Modules[Dependency];

		// Add both include and generated directories
		BuildCmd.push_back("/I\"" + Mod->GetIncludeDir() + "\\\"");
		BuildCmd.push_back("/I\"" + Mod->GetGeneratedDir() + "\\\"");
	}

	// Add custom include paths
	for (const std::string& IncludePath : TheModule.PythonIncludes)
	{
		BuildCmd.push_back("/I\"" + IncludePath + "\\\\\"");
	}

	// Add external includes
	for (const ExternDependency& Dep : TheModule.ExternDependencies)
	{
		BuildCmd.push_back("/I\"" + Dep.GetIncludePath() + "\\\\\"");
	}

	// Add third party include paths to include path
	for (const std::string& ThirdPartyInclude : TheModule.PythonIncludes)
	{
		BuildCmd.push_back("/I\"" + (Filesystem::path(TheModule.GetThirdPartyDir()) / ThirdPartyInclude).string() + "\\\"");
	}

	// Only mark the specified source file for compilation
	BuildCmd.push_back("\"" + SourceFile + "\"");

	// Allocate buffer for standard error
	int StdOutSize = 1024 * 1000;
	char* StdOutBuffer = new char[StdOutSize];
	char* StdErrBuffer = new char[StdOutSize];

	bool bResult = ExecProc(Program, BuildCmd, StdOutSize, StdOutBuffer, StdOutSize, StdErrBuffer);

	// Set the std err string 
	StdErr = StdErrBuffer;
	StdOut = StdOutBuffer;

	// For visual studio, chop off first line since that just repeats the source file

	int NewLine = StdOut.find_first_of("\r\n");

	if(NewLine != std::string::npos)
	{
		StdOut = StdOut.substr(NewLine + 2, StdOut.size() - (NewLine + 2) - 2); // Also chop off last new line
	}

	// Fire up the compilation process with the built command
	return bResult;
}

bool MSVCBuildTool::LinkModule(Module& TheModule)
{
	std::string Program = LinkPath;
	
	std::vector<std::string> BuildCmd;

	std::string ArtifactName = TheModule.GetArtifactName();
	std::string ArtifactDllName = ArtifactName + ".dll";
	std::string ArtifactLibName = ArtifactName + ".lib";
	std::string ArtifactPdbName = ArtifactName + ".pdb";
	std::string ArtifactExeName = ArtifactName + ".exe";

	std::string ModBinDir = GetModuleBinaryDir(TheModule);
	std::string ModLibDir = GetModuleLibraryDir(TheModule);
	std::string ModObjDir = GetModuleObjectDir(TheModule);

	Filesystem::path ArtifactPDBPath = Filesystem::absolute(Filesystem::path(ModBinDir) / ArtifactPdbName);
	Filesystem::path ArtifactLibPath = Filesystem::absolute(Filesystem::path(ModLibDir) / ArtifactLibName);
	Filesystem::path ArtifactDLLPath = Filesystem::absolute(Filesystem::path(ModBinDir) / ArtifactDllName);
	Filesystem::path ArtifactExePath = Filesystem::absolute(Filesystem::path(ModBinDir) / ArtifactExeName);

	// See if we need to build a DLL
	if (!TheModule.IsExecutable(Settings))
	{
		BuildCmd.push_back("/DLL");
	}

	// Create debug information for the code.

	if(Settings.Config == BuildConfiguration::DEVELOPMENT)
	{
		BuildCmd.push_back("/DEBUG");
	}

	// Set up library paths
	{
		// Add the passed in libraries directory so we can automatically link with the other built modules
		BuildCmd.push_back("/LIBPATH:\"" + ModLibDir + "\\\\\" ");

		// Add the passed in intermediate directory so we can add the obj files as input
		BuildCmd.push_back("/LIBPATH:\"" + ModObjDir + "\\\\\" ");

		// If we're not an engine module, we need to add the engine library paths
		if(!TheModule.bEngineModule)
		{
			BuildCmd.push_back("/LIBPATH:\"" + GetEngineLibraryDir() + "\\\\\" ");
		}

		for (const ExternDependency& Lib : TheModule.ExternDependencies)
		{
			BuildCmd.push_back("/LIBPATH:\"" + Lib.GetPlatformLibraryPath(Settings) + "\\\\\" ");
		}

		// Add third party library directories
		for (const std::string& Lib : TheModule.PythonLibraryPaths)
		{
			// Make path relative to third party
			// todo: don't force relative to third party
			Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(TheModule.GetThirdPartyDir()) / Lib);

			BuildCmd.push_back("/LIBPATH:\"" + LibPath.string() + "\\\" ");
		}

	}

	// Set the name of the output file depending on if we're building a shared library or an executable.
	std::string OutputOption = "/OUT:\"";
	{
		if (TheModule.IsExecutable(Settings))
		{
			OutputOption += ArtifactExePath.string();
		}
		else
		{
			OutputOption += ArtifactDLLPath.string();
		}
	}
	OutputOption += "\" ";
	BuildCmd.push_back(OutputOption);

	// Set the name of the import library
	if (!TheModule.IsExecutable(Settings))
	{
		// Relocate the import library
		BuildCmd.push_back("/IMPLIB:\"" + ArtifactLibPath.string() + "\"");
	}

	for (const ExternDependency& Extern : TheModule.ExternDependencies)
	{
		std::vector<std::string> Libs;
		Extern.GetPlatformLibs(Settings, Libs);
		for (const std::string& ExternLib : Libs)
		{
			BuildCmd.push_back(ExternLib);
		}
	}

	// Add all libraries as input. This works for static as well as shared (dll) libraries.
	for (const std::string& SystemLib : TheModule.PythonLibraries)
	{
		BuildCmd.push_back(SystemLib);
	}

	// Add the dependency library names. The passed in lib path was already added earlier so these paths are already covered.
	for (const std::string& Dependency : TheModule.ModuleDependencies)
	{
		Module* Mod = Modules[Dependency];

		std::string DepLibName = Mod->GetArtifactName() + ".lib";
		Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(ModLibDir) / DepLibName);

		BuildCmd.push_back(DepLibName);
	}

	// Find all OBJ files and add them as input
	std::vector<std::string> FoundObjFiles;
	std::vector<std::string> MissingObjFiles;
	TheModule.DiscoverObjectFiles(ModObjDir, ".obj", FoundObjFiles, MissingObjFiles);

	if (!MissingObjFiles.empty())
	{
		std::cerr << "One or more obj files are missing, please ensure program compiled correctly before linking" << std::endl;
		return false;
	}

	for (std::string ObjFile : FoundObjFiles)
	{
		BuildCmd.push_back(Filesystem::path(ObjFile).filename().string());
	}

	std::cout << "Linking module " << TheModule.Name << std::endl;

	// Invoke the compiler to link the module.
	int StdOutBuffSize = 1024 * 1000;
	char* StdOutBuff = new char[StdOutBuffSize];

	bool bLinkResult = ExecProc(Program, BuildCmd, StdOutBuffSize, StdOutBuff);

	if (bLinkResult)
	{
		std::cout << "Module " << TheModule.Name << " linked successfully" << std::endl;
	}
	else
	{
		// Print out link error message
		std::cerr << StdOutBuff << std::endl;

		// Set this module to build failed
		TheModule.bBuiltSuccessfully = false;
	}

	return bLinkResult;
}

bool MSVCBuildTool::LinkStandalone(std::string OutputDirectory, std::string ObjectDirectory, std::string StandaloneName)
{
	std::string Program = LinkPath;

	std::vector<std::string> BuildCmd;

	std::string ArtifactName = StandaloneName;
	std::string ArtifactPdbName = ArtifactName + ".pdb";
	std::string ArtifactExeName = ArtifactName + ".exe";

	Filesystem::path ArtifactPDBPath = Filesystem::absolute(Filesystem::path(OutputDirectory) / ArtifactPdbName);
	Filesystem::path ArtifactExePath = Filesystem::absolute(Filesystem::path(OutputDirectory) / ArtifactExeName);

	// Create debug information for the code.
	if(Settings.Config == BuildConfiguration::DEVELOPMENT)
	{
		BuildCmd.push_back("/DEBUG");
	}

	BuildCmd.push_back("/SUBSYSTEM:WINDOWS");

	// Set up library paths
	{
		// Add the passed in intermediate directory so we can add the obj files as input
		BuildCmd.push_back("/LIBPATH:\"" + ObjectDirectory + "\\\\\" ");
		
		// Add third party library directories
		// Add all third party library paths together

		for(auto& Mod : Modules)
		{
			Module& TheModule = *Mod.second;

			for (const ExternDependency& Lib : TheModule.ExternDependencies)
			{
				BuildCmd.push_back("/LIBPATH:\"" + Lib.GetPlatformLibraryPath(Settings) + "\\\\\" ");
			}

			for (const std::string& Lib : TheModule.PythonLibraryPaths)
			{
				// Make path relative to third party
				Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(TheModule.GetThirdPartyDir()) / Lib);

				BuildCmd.push_back("/LIBPATH:\"" + LibPath.string() + "\\\" ");
			}
			
		}

	}

	// Set the name of the output file depending on if we're building a shared library or an executable.
	BuildCmd.push_back("/OUT:\"" + ArtifactExePath.string() + "\" ");

	// Include ALL module libraries
	for (auto& Mod : Modules)
	{
		Module& TheModule = *Mod.second;

		for (const ExternDependency& Extern : TheModule.ExternDependencies)
		{
			std::vector<std::string> Libs;
			Extern.GetPlatformLibs(Settings, Libs);
			for (const std::string& ExternLib : Libs)
			{
				BuildCmd.push_back(ExternLib);
			}
		}
		
		for (const std::string& SystemLib : TheModule.PythonLibraries)
		{
			BuildCmd.push_back(SystemLib);
		}
		
	}

	// Find all OBJ files and add them as input
	std::vector<std::string> FoundObjFiles;
	std::vector<std::string> MissingObjFiles;

	for (auto& Mod : Modules)
	{
		Module& TheModule = *Mod.second;
		TheModule.DiscoverObjectFiles(ObjectDirectory, ".obj", FoundObjFiles, MissingObjFiles);
	}

	if (!MissingObjFiles.empty())
	{
		std::cerr << "One or more obj files are missing, please ensure program compiled correctly before linking" << std::endl;
		return false;
	}

	for (std::string ObjFile : FoundObjFiles)
	{
		BuildCmd.push_back(Filesystem::path(ObjFile).filename().string());
	}

	std::cout << "Linking standalone " << StandaloneName << std::endl;

	// Invoke the compiler to link the module.
	int StdOutBuffSize = 1024 * 1000;
	char* StdOutBuff = new char[StdOutBuffSize];

	bool bLinkResult = ExecProc(Program, BuildCmd, StdOutBuffSize, StdOutBuff);

	if (bLinkResult)
	{
		std::cout << "Standalone " << StandaloneName << " linked successfully" << std::endl;

		return true;
	}
	else
	{
		// Print out link error message
		std::cerr << StdOutBuff << std::endl;

		return false;
	}
}

#endif