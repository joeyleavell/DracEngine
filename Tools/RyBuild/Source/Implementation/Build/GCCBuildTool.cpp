#include "Build/GCCBuildTool.h"
#include "Common.h"

#include "Build/Module/Module.h"
#include <map>
#include "Util/Util.h"
#include <iostream>
#include <fstream>
#include "Common.h"
#include <algorithm>
#include <set>

GCCBuildTool::GCCBuildTool(std::string RootDir, BuildSettings Settings) :
AbstractBuildTool(RootDir, Settings)
{
	if(Settings.TargetPlatform.OS == TargetOS::Linux)
	{
		this->ObjectFileExtension = ".o";
	}
	else if(Settings.TargetPlatform.OS == TargetOS::Windows)
	{
		this->ObjectFileExtension = ".obj";
	}

	// Determine the program to use based on target/host platforms
	Program = "g++";
	if (Settings.HostPlatform.OS == TargetOS::Linux)
	{
		if (Settings.HostPlatform.Arch == TargetArchitecture::x86_64)
		{
			if (Settings.TargetPlatform.OS == TargetOS::Windows)
			{
				if (Settings.TargetPlatform.Arch == TargetArchitecture::x86_64)
				{
					Program = "x86_64-w64-mingw32-g++";
				}
			}
		}
	}
}

bool GCCBuildTool::BuildSingleSource(const Module& TheModule, std::string OutputDirectory, std::string SourceFile, std::string& StdOut, std::string& StdErr)
{
	// Determine which program to use based on target/host platform combo
	std::string ModBinDir = GetModuleBinaryDir(TheModule);
	std::string ModLibDir = GetModuleLibraryDir(TheModule);
	//std::string ModObjDir = GetModuleObjectDir(TheModule);

	// Specify compile only
	std::vector<std::string> CmdArgs;

	CmdArgs.push_back("-c");

	CmdArgs.push_back("-Wall");

	// Create output file name
	std::string OutputFileName = Filesystem::path(SourceFile).stem().string() + ObjectFileExtension;

	// On Linux, if we're compiling a shared library, we need to specify position independent code.
	if(Settings.TargetPlatform.OS == TargetOS::Linux)
	{
		if (!TheModule.IsExecutable(Settings))
		{
			CmdArgs.push_back("-fpic");
		}
	}

	// Setup the intermediate directory for output of the OBJ
	CmdArgs.push_back("-o");
	CmdArgs.push_back((Filesystem::path(OutputDirectory) / OutputFileName).string());

	// By default linux libraries are using the "shared" version, so this doesn't have an equivalent with g++
	// BuildCmd += "/MD ";

	// Debug information (debug builds only?)
	if(Settings.Config == BuildConfiguration::Development)
	{
		CmdArgs.push_back("-g");
	}

	// Define macro for target platform
	CmdArgs.push_back("-DRYBUILD_CONFIG_" + Settings.ConfigToString());
	CmdArgs.push_back("-DRYBUILD_TARGET_" + Settings.TargetOSToString());

	if (Settings.bDistribute)
	{
		CmdArgs.push_back("-DRYBUILD_DISTRIBUTE");
	}

	if(Settings.Type == BuildType::Standalone)
	{
		CmdArgs.push_back("-DRYBUILD_STANDALONE");
	}

	// Need to specify the windows version on MinGW else we won't have access to certain functions
	if(Settings.TargetPlatform.OS == TargetOS::Windows)
	{
		CmdArgs.push_back("-D_WIN32_WINNT=0x0502");
	}

	// Define any preprocessor symbols here
	{	
		// Register any extra macro definition requests by the module
		for (const std::string& MacroDef : TheModule.MacroDefinitions)
		{
			CmdArgs.push_back("-D" + MacroDef);
		}
	
		CmdArgs.push_back("-DCOMPILE_MODULE_" + ToUpper(TheModule.Name));
	}

	// Set c++ to version 17
	CmdArgs.push_back("-std=c++17");
	
	// Add the module's include path
	CmdArgs.push_back("-I" + TheModule.GetIncludeDir());

	// Add the module's generated path
	CmdArgs.push_back("-I" + TheModule.GetGeneratedDir());

	// Add module dependencies to include path
	for (const std::string& Dependency : TheModule.ModuleDependencies)
	{
		Module* Mod = Modules[Dependency];
	
		// Add both include and generated directories
		CmdArgs.push_back("-I" + Mod->GetIncludeDir());
		CmdArgs.push_back("-I" + Mod->GetGeneratedDir());
	}

	// Add external includes
	for (const std::string& IncludePath : TheModule.PythonIncludes)
	{
		CmdArgs.push_back("-I" + IncludePath);
	}

	for (const ExternDependency& Extern : TheModule.ExternDependencies)
	{
		CmdArgs.push_back("-I" + Extern.GetIncludePath());
	}

	// Add third party include paths to include path
	// for (const std::string& ThirdPartyInclude : TheModule.ModuleThirdParty.Includes)
	// {
	// 	CmdArgs.push_back("-I" + (Filesystem::path(TheModule.GetThirdPartyDir()) / ThirdPartyInclude).string());
	// }

	// Only mark the specified source file for compilation
	CmdArgs.push_back(SourceFile);

	constexpr int BufferSize = 1024 * 100;
	char StdOutBuffer[BufferSize], StdErrBuffer[BufferSize];

	bool bCompileResult = ExecProc(Program, CmdArgs, BufferSize, StdOutBuffer, BufferSize, StdErrBuffer);

	StdOut = StdOutBuffer;
	StdErr = StdErrBuffer;

	return bCompileResult;
}

bool GCCBuildTool::LinkModule(Module& TheModule)
{	
	std::string ModBinDir = GetModuleBinaryDir(TheModule);
	std::string ModLibDir = GetModuleLibraryDir(TheModule);
	std::string ModObjDir = GetModuleObjectDir(TheModule);

	Filesystem::create_directories(ModLibDir);
	Filesystem::create_directories(ModObjDir);

	std::vector<std::string> CmdArgs;

	std::string ArtifactName = TheModule.GetArtifactName();
	std::string ArtifactSharedLibName = "";
	std::string ArtifactImpLibName = ArtifactName + ".lib"; // This value is only used on windows targets
	std::string ArtifactExecutableName = "";

	// TODO: How to generate debug info with gcc?
	// 
	// std::string ArtifactPdbName = ArtifactName + ".pdb";

	if(Settings.TargetPlatform.OS == TargetOS::Windows)
	{
		ArtifactExecutableName = ArtifactName + ".exe";
		ArtifactSharedLibName = ArtifactName + ".dll";
	}
	else if(Settings.TargetPlatform.OS == TargetOS::Linux)
	{
		ArtifactExecutableName = ArtifactName + ".out";
		ArtifactSharedLibName = "lib" + ArtifactName + ".so";
	}

	// Tell GCC to statically link libgcc and libstdc++ if on Windows
	// This prevents people from needing to download mingw binaries
	if(Settings.TargetPlatform.OS == TargetOS::Windows)
	{
		CmdArgs.push_back("-static");
		CmdArgs.push_back("-static-libstdc++");
		CmdArgs.push_back("-static-libgcc");
	}

	// Filesystem::path ArtifactPDBPath = Filesystem::absolute(Filesystem::path(BinaryDir) / ArtifactPdbName);
	Filesystem::path ArtifactSharedLibPath = Filesystem::canonical(Filesystem::path(ModBinDir)) / ArtifactSharedLibName;
	Filesystem::path ArtifactImpLibPath = Filesystem::canonical(Filesystem::path(ModLibDir)) / ArtifactImpLibName;
	Filesystem::path ArtifactExePath = Filesystem::canonical(Filesystem::path(ModBinDir)) / ArtifactExecutableName;

	std::string LinkerOptions = "";

	// Tell linker to search in working directory for shared objects (binary folder) if on Linux
	if(Settings.TargetPlatform.OS == TargetOS::Linux)
	{
		LinkerOptions += "-Wl,-rpath,${ORIGIN}";
	}

	// See if we need to build a DLL
	if (!TheModule.IsExecutable(Settings))
	{
		// If on windows, we need to give GCC a name for the import library
		if(Settings.TargetPlatform.OS == TargetOS::Windows)
		{
			LinkerOptions += "-Wl,--out-implib," + ArtifactImpLibPath.string();
		}
		else if(Settings.TargetPlatform.OS == TargetOS::Linux)
		{
			// Setup the soname
			LinkerOptions += ",-soname," + ArtifactSharedLibName;
		}
		
		CmdArgs.push_back("-shared");
	}

	if(!LinkerOptions.empty())
	{
		CmdArgs.push_back(LinkerOptions);
	}


	// Set up library paths
	{
		CmdArgs.push_back("-L" + ModLibDir);

		CmdArgs.push_back("-L" + ModObjDir);

		CmdArgs.push_back("-L" + ModBinDir);

		if (!TheModule.bEngineModule)
		{
			CmdArgs.push_back("-L" + GetEngineLibraryDir());
		}

		// std::vector<std::string>* Libs = TheModule.GetTargetLibPaths(Settings);
		std::vector<std::string> Libs = TheModule.PythonLibraries;

		// if(Libs)
		// {
			// Add third party libraries
			for (const std::string& Lib : Libs)
			{
				// Make path relative to third party
				Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(TheModule.GetThirdPartyDir()) / Lib);

				CmdArgs.push_back("-L" + LibPath.string());
			}
		// }
		// else
		// {
		// 	std::cerr << "Module did not return a valid vector of target libs" << std::endl;
		// 	return false;
		// }
	
	}

	// Set the name of the output file depending on if we're building a shared library or an executable.
	if (TheModule.IsExecutable(Settings))
	{
		CmdArgs.push_back("-o" + ArtifactExePath.string());
	}
	else
	{
		CmdArgs.push_back("-o" + ArtifactSharedLibPath.string());
	}

	// Find all object files and add them as input
	std::vector<std::string> FoundObjFiles;
	std::vector<std::string> MissingObjFiles;
	TheModule.DiscoverObjectFiles(ModObjDir, ObjectFileExtension, FoundObjFiles, MissingObjFiles);

	if(MissingObjFiles.size() > 0)
	{
		std::cerr << "One or more obj files are missing, please ensure program compiled correctly before linking" << std::endl;
		return false;
	}

	for(std::string ObjFile : FoundObjFiles)
	{
		CmdArgs.push_back(ObjFile);
	}

	std::vector<std::string> ModulesToLink;
	std::vector<std::string> DepsTopOrder;

	if(TheModule.IsExecutable(Settings))
	{
		// Do a full recurse to find all dependent modules
		RecurseDependencies(TheModule, Modules, ModulesToLink);
	}
	else
	{
		ModulesToLink = TheModule.ModuleDependencies;
	}

	TopSort(ModulesToLink, Modules, DepsTopOrder);

	// Add the modules to the link list in their topological order
	for (const std::string& ModDep : DepsTopOrder)
	{
		Module* Mod = Modules[ModDep];

		std::string DepLibName = Mod->GetArtifactName();
		// Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(LibraryDir) / DepLibName);

		CmdArgs.push_back("-l" + DepLibName); /*Use so extension*/
	}


	// Libraries that will be added to the compile path
	std::vector<std::string> TargetLibs = TheModule.PythonLibraries;
	//TheModule.GetTargetLibs(Settings, TargetLibs);

	// Add all extern library inputs
	for (const ExternDependency& Extern : TheModule.ExternDependencies)
	{
		Extern.GetPlatformLibs(Settings, TargetLibs);
	}

	for (const std::string& Library : TargetLibs)
	{
		// Take only the stem (part prior to extension). This is in case Windows libs have a.lib, it'll only take the "a".
		std::string LibStemmed = Filesystem::path(Library).stem().string();

		CmdArgs.push_back("-l" + LibStemmed);
	}

	for(std::string s : CmdArgs)
	{
		std::cout << s << std::endl;
	}

	std::cout << "Linking module " << TheModule.Name << std::endl;

	// if (bIsVerbose)
	// {
	// }

	// Invoke the compiler to link the module.
	constexpr int BufferSize = 1024 * 100;
	char StdOutBuff[BufferSize], StdErrBuff[BufferSize];

	bool bLinkResult = ExecProc(Program, CmdArgs, BufferSize, StdOutBuff, BufferSize, StdErrBuff);

	if (bLinkResult)
	{
		std::cout << "Module " << TheModule.Name << " linked successfully" << std::endl;
	}
	else
	{
		// Print out link error message
		std::cerr << StdOutBuff << std::endl;
		std::cerr << StdErrBuff << std::endl;

		// Set this module to build failed
		TheModule.bBuiltSuccessfully = false;
	}

	return bLinkResult;
}

bool GCCBuildTool::LinkStandalone(std::string OutputDirectory, std::string ObjectDirectory, std::string StandaloneName)
{
	std::vector<std::string> CmdArgs;

	std::string ArtifactName = StandaloneName;
	std::string ArtifactExecutableName = "";

	// TODO: How to generate debug info with gcc?
	// 
	// std::string ArtifactPdbName = ArtifactName + ".pdb";

	if (Settings.TargetPlatform.OS == TargetOS::Windows)
	{
		ArtifactExecutableName = ArtifactName + ".exe";
	}
	else if (Settings.TargetPlatform.OS == TargetOS::Linux)
	{
		ArtifactExecutableName = ArtifactName + ".out";
	}

	// Filesystem::path ArtifactPDBPath = Filesystem::absolute(Filesystem::path(BinaryDir) / ArtifactPdbName);
	Filesystem::path ArtifactExePath = Filesystem::canonical(Filesystem::path(OutputDirectory)) / ArtifactExecutableName;

	// Tell GCC to statically link libgcc and libstdc++ if on Windows
	if (Settings.TargetPlatform.OS == TargetOS::Windows)
	{
		CmdArgs.push_back("-static-libstdc++");
		CmdArgs.push_back("-static-libgcc");
	}

	std::string LinkerOptions = "";

	// Tell linker to search in working directory for shared objects (binary folder) if on Linux
	if (Settings.TargetPlatform.OS == TargetOS::Linux)
	{
		LinkerOptions += "-Wl,-rpath,${ORIGIN}";
	}

	if (!LinkerOptions.empty())
	{
		CmdArgs.push_back(LinkerOptions);
	}

	// Set up library paths
	{
		CmdArgs.push_back("-L" + ObjectDirectory);

		for (auto& Mod : Modules)
		{
			Module& TheModule = *Mod.second;
			// std::vector<std::string>* Libs = TheModule.GetTargetLibPaths(Settings);
			std::vector<std::string> Libs = TheModule.PythonLibraryPaths;

			// if (Libs)
			// {
				// Add third party libraries
				for (const std::string& Lib : Libs)
				{
					// Make path relative to third party
					Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(TheModule.GetThirdPartyDir()) / Lib);

					CmdArgs.push_back("-L" + LibPath.string());
				}
			// }
			// else
			// {
			// 	std::cerr << "Module did not return a valid vector of target libs" << std::endl;
			// 	return false;
			// }
		}

	}

	CmdArgs.push_back("-o" + ArtifactExePath.string());

	// Find all object files and add them as input
	std::vector<std::string> FoundObjFiles;
	std::vector<std::string> MissingObjFiles;

	for (auto& Mod : Modules)
	{
		Module& TheModule = *Mod.second;
		TheModule.DiscoverObjectFiles(ObjectDirectory, ObjectFileExtension, FoundObjFiles, MissingObjFiles);
	}

	if (MissingObjFiles.size() > 0)
	{
		std::cerr << "One or more obj files are missing, please ensure program compiled correctly before linking" << std::endl;
		return false;
	}

	for (std::string ObjFile : FoundObjFiles)
	{
		CmdArgs.push_back(ObjFile);
	}

	// Add all library inputs
	for (auto& Mod : Modules)
	{
		Module& TheModule = *Mod.second;

		// Todo: combine shared code between here and LinkModule()
		
		// Libraries that will be added to the compile path
		std::vector<std::string> TargetLibs = TheModule.PythonLibraries;
		// TheModule.GetTargetLibs(Settings, TargetLibs);

		// Add all extern library inputs
		for (const ExternDependency& Extern : TheModule.ExternDependencies)
		{
			Extern.GetPlatformLibs(Settings, TargetLibs);
		}

		for (const std::string& Library : TargetLibs)
		{
			// Take only the stem (part prior to extension). This is in case Windows libs have a.lib, it'll only take the "a".
			std::string LibStemmed = Filesystem::path(Library).stem().string();

			CmdArgs.push_back("-l" + LibStemmed);
		}
	}

	// for (std::string s : CmdArgs)
	// {
	// 	std::cout << s << std::endl;
	// }

	for (std::string s : CmdArgs)
	{
		std::cout << s << std::endl;
	}
	
	std::cout << "Linking standalone " << StandaloneName << std::endl; 
	 
	// Invoke the compiler to link the module.
	constexpr int BufferSize = 1024 * 100;
	char StdOutBuff[BufferSize], StdErrBuff[BufferSize];

	bool bLinkResult = ExecProc(Program, CmdArgs, BufferSize, StdOutBuff, BufferSize, StdErrBuff);

	if (bLinkResult)
	{
		std::cout << "Standalone " << StandaloneName << " linked successfully" << std::endl;

		return true;
	}
	else
	{
		// Print out link error message
		std::cerr << StdOutBuff << std::endl;
		std::cerr << StdErrBuff << std::endl;

		// Set this module to build failed
		return false;
	}
	
}