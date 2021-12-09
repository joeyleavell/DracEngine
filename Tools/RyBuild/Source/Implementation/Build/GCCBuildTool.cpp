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
	if(Settings.TargetPlatform.OS == OSType::LINUX)
	{
		this->ObjectFileExtension = ".o";
	}
	else if(Settings.TargetPlatform.OS == OSType::WINDOWS)
	{
		this->ObjectFileExtension = ".obj";
	}
	else if(Settings.TargetPlatform.OS == OSType::OSX)
	{
		this->ObjectFileExtension = ".o";
	}


	// Determine the program to use based on target/host platforms
	Program = "g++";
	if (Settings.HostPlatform.OS == OSType::LINUX)
	{
		if (Settings.HostPlatform.Arch == ArchitectureType::X64)
		{
			if (Settings.TargetPlatform.OS == OSType::WINDOWS)
			{
				if (Settings.TargetPlatform.Arch == ArchitectureType::X64)
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
	CmdArgs.push_back("-Wno-invalid-offsetof");

	// Create output file name
	std::string OutputFileName = Filesystem::path(SourceFile).stem().string() + ObjectFileExtension;

	// On Linux, if we're compiling a shared library, we need to specify position independent code.
	if(Settings.TargetPlatform.OS == OSType::LINUX || Settings.TargetPlatform.OS == OSType::OSX)
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
	if(Settings.Config == BuildConfiguration::DEVELOPMENT)
	{
		CmdArgs.push_back("-g");
	}

	// Define macro for target platform
	CmdArgs.push_back("-DRYBUILD_CONFIG_" + Settings.ConfigToString());
	CmdArgs.push_back("-DRBUILD_TARGET_OS_" + ToUpper(OSToString(Settings.TargetPlatform.OS)));

	if (Settings.bDistribute)
	{
		CmdArgs.push_back("-DRYBUILD_DISTRIBUTE");
	}

	if(Settings.Type == BuildType::STANDALONE)
	{
		CmdArgs.push_back("-DRYBUILD_STANDALONE");
	}

	// Need to specify the windows version on MinGW else we won't have access to certain functions
	if(Settings.TargetPlatform.OS == OSType::WINDOWS)
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
	std::vector<std::string> ModulesToLink;
	std::vector<std::string> DepsTopOrder;
	std::set<std::string> TargetLibs;
	std::set<std::string> TargetLibraryPaths;	
	std::vector<ExternDependency> ExternalDependencies;

	if (TheModule.IsExecutable(Settings))
	{
		// Do a full recurse to find all dependent modules
		RecurseDependencies(TheModule, Modules, ModulesToLink);

		// Add external libs for every module so gcc linker doesn't complain
		for (std::string& DownstreamModule : ModulesToLink)
		{
			Module* ModDep = Modules.at(DownstreamModule);

			// Add all external dependencies
			for (const ExternDependency& Extern : ModDep->ExternDependencies)
			{
				ExternalDependencies.push_back(Extern);
			}

			// Add all library paths
			for (std::string& LibPath : ModDep->PythonLibraryPaths)
			{
				// Make path relative to third party
				Filesystem::path LibPathRel = Filesystem::absolute(Filesystem::path(ModDep->GetThirdPartyDir()) / LibPath);
				TargetLibraryPaths.insert(LibPathRel.string());
			}

		}
	}
	else
	{
		ModulesToLink = TheModule.ModuleDependencies;
		ExternalDependencies = TheModule.ExternDependencies;

		for (std::string& Lib : TheModule.PythonLibraries)
		{
			TargetLibs.insert(Lib);
		}

		for(std::string& LibPath : TheModule.PythonLibraryPaths)
		{
			// Make path relative to third party
			Filesystem::path LibPathRel = Filesystem::absolute(Filesystem::path(TheModule.GetThirdPartyDir()) / LibPath);
			TargetLibraryPaths.insert(LibPathRel.string());
		}
	}

	// Add the libs of all of the external dependencies to the target libraries
	for (const ExternDependency& Extern : ExternalDependencies)
	{
		std::vector<std::string> ExternPlatformLibs;

		Extern.GetPlatformLibs(Settings, ExternPlatformLibs);

		// Also need to check for bins on Linux
		if(Settings.TargetPlatform.OS == OSType::LINUX || Settings.TargetPlatform.OS == OSType::OSX)
		{
			Extern.GetPlatformBins(Settings, ExternPlatformLibs);
		}

		for(std::string& ExternLib : ExternPlatformLibs)
		{
			TargetLibs.insert(ExternLib);
		}
	}

	// Do a topological sort of the module dependencies
	// This is required since GCC expects a certain link order
	// If Module A depends on Module B, Module A must appear first
	// So, it's sort of a reverse topoglogical sort
	TopSort(ModulesToLink, Modules, DepsTopOrder);

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

	if(Settings.TargetPlatform.OS == OSType::WINDOWS)
	{
		ArtifactExecutableName = ArtifactName + ".exe";
		ArtifactSharedLibName = ArtifactName + ".dll";
	}
	else if(Settings.TargetPlatform.OS == OSType::LINUX)
	{
		ArtifactExecutableName = ArtifactName + ".out";
		ArtifactSharedLibName = "lib" + ArtifactName + ".so";
	}
	else if(Settings.TargetPlatform.OS == OSType::OSX)
	{
		ArtifactExecutableName = ArtifactName;
		ArtifactSharedLibName = "lib" + ArtifactName + ".dylib";
	}

	// Tell GCC to statically link libgcc and libstdc++ if on Windows
	// This prevents people from needing to download mingw binaries
	if(Settings.TargetPlatform.OS == OSType::WINDOWS)
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

	// See if we need to build a DLL
	if (!TheModule.IsExecutable(Settings))
	{
		// If on windows, we need to give GCC a name for the import library
		if(Settings.TargetPlatform.OS == OSType::WINDOWS)
		{
			LinkerOptions += "-Wl,--out-implib," + ArtifactImpLibPath.string();
		}
		else if(Settings.TargetPlatform.OS == OSType::LINUX)
		{
			// Setup the rpath and soname
			LinkerOptions += "-Wl,-rpath,${ORIGIN},-soname," + ArtifactSharedLibName;
		}
		else if(Settings.TargetPlatform.OS == OSType::OSX)
		{
			// Setup the install name and rpath
			LinkerOptions += "-Wl,-install_name,@rpath/" + ArtifactSharedLibName;
		}

		if(Settings.TargetPlatform.OS == OSType::OSX)
		{
			CmdArgs.push_back("-dynamiclib");
		}
		else
		{
			CmdArgs.push_back("-shared");
		}
	}
	else
	{
		if(Settings.TargetPlatform.OS == OSType::OSX)
		{
			LinkerOptions += "-Wl,-rpath,@executable_path";
		}
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

		for (const std::string& Lib : TargetLibraryPaths)
		{
			CmdArgs.push_back("-L" + Lib);
		}

		for (const ExternDependency& Extern : ExternalDependencies)
		{
			CmdArgs.push_back("-L" + Extern.GetPlatformLibraryPath(Settings));

			// Also need to add binary path for Linux (SOs) and OSX (dylibs)
			if (Settings.TargetPlatform.OS == OSType::LINUX || Settings.TargetPlatform.OS == OSType::OSX)
			{
				CmdArgs.push_back("-L" + Extern.GetPlatformBinaryPath(Settings));
			}
			else
			{
			}
		}
		
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

	// Add the modules to the link list in their topological order
	for (const std::string& ModDep : DepsTopOrder)
	{
		Module* Mod = Modules[ModDep];

		std::string DepLibName = Mod->GetArtifactName();
		// Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(LibraryDir) / DepLibName);

		CmdArgs.push_back("-l" + DepLibName); /*Use so extension*/
	}

	for (const std::string& Library : TargetLibs)
	{
		// Take only the stem (part prior to extension). This is in case Windows libs have a.lib, it'll only take the "a".
		std::string LibStemmed = Filesystem::path(Library).stem().string();

		// Remove lib prefix
		if(LibStemmed.find("lib") == 0)
		{
			LibStemmed = LibStemmed.substr(3);
		}
		
		CmdArgs.push_back("-l" + LibStemmed);
	}

	// Add OSX frameworks
	if(Settings.TargetPlatform.OS == OSType::OSX)
	{
		for (const std::string& Framework : TheModule.OSXFrameworks)
		{
			CmdArgs.push_back("-framework");
			CmdArgs.push_back(Framework);
		}
	}

	std::cout << "Linking module " << TheModule.Name << std::endl;

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

	if (Settings.TargetPlatform.OS == OSType::WINDOWS)
	{
		ArtifactExecutableName = ArtifactName + ".exe";
	}
	else if (Settings.TargetPlatform.OS == OSType::LINUX)
	{
		ArtifactExecutableName = ArtifactName + ".out";
	}
	else if (Settings.TargetPlatform.OS == OSType::OSX)
	{
		ArtifactExecutableName = ArtifactName;
	}

	// Filesystem::path ArtifactPDBPath = Filesystem::absolute(Filesystem::path(BinaryDir) / ArtifactPdbName);
	Filesystem::path ArtifactExePath = Filesystem::canonical(Filesystem::path(OutputDirectory)) / ArtifactExecutableName;

	// Tell GCC to statically link libgcc and libstdc++ if on Windows
	if (Settings.TargetPlatform.OS == OSType::WINDOWS)
	{
		CmdArgs.push_back("-static-libstdc++");
		CmdArgs.push_back("-static-libgcc");
	}

	std::string LinkerOptions = "";

	// Tell linker to search in working directory for shared objects (binary folder) if on Linux
	if (Settings.TargetPlatform.OS == OSType::LINUX || Settings.TargetPlatform.OS == OSType::OSX)
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

			std::vector<std::string> Libs = TheModule.PythonLibraryPaths;
			for (const std::string& Lib : Libs)
			{
				// Make path relative to third party
				Filesystem::path LibPath = Filesystem::absolute(Filesystem::path(TheModule.GetThirdPartyDir()) / Lib);

				CmdArgs.push_back("-L" + LibPath.string());
			}

			for (const ExternDependency& Extern : TheModule.ExternDependencies)
			{
				CmdArgs.push_back("-L" + Extern.GetPlatformLibraryPath(Settings));

				// Also need to add bins path for SOs on Linux
				if(Settings.TargetPlatform.OS == OSType::LINUX || Settings.TargetPlatform.OS == OSType::OSX)
				{
					CmdArgs.push_back("-L" + Extern.GetPlatformBinaryPath(Settings));
				}
				
			}

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

			// Also need to check for SOs on Linux
			if(Settings.TargetPlatform.OS == OSType::LINUX || Settings.TargetPlatform.OS == OSType::OSX)
			{
				Extern.GetPlatformBins(Settings, TargetLibs);
			}
		}

		// Todo: put this into its own function
		// It's also used in LinkModule
		for (const std::string& Library : TargetLibs)
		{
			// Take only the stem (part prior to extension). This is in case Windows libs have a.lib, it'll only take the "a".
			std::string LibStemmed = Filesystem::path(Library).stem().string();

			// Remove lib prefix
			if (LibStemmed.find("lib") == 0)
			{
				LibStemmed = LibStemmed.substr(3);
			}

			CmdArgs.push_back("-l" + LibStemmed);
		}
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