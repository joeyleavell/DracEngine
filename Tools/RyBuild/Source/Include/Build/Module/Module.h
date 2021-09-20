#pragma once

#include <string>
#include <vector>
#include "Common.h"
#include <map>
#include <set>
#include "Build/BuildSettings.h"
#include "Json/json.hpp"

typedef nlohmann::json Json;

struct WindowsTargetLibs
{
	std::vector<std::string> MSVCLibs;
	std::vector<std::string> MinGWLibs;
};

struct LinuxTargetLibs
{
	std::vector<std::string> GCCLibs;
};

struct PlatformThirdParty
{
	std::vector<std::string> Includes;

	LinuxTargetLibs Linux64Libs;
	LinuxTargetLibs Linux32Libs;
	WindowsTargetLibs Win64Libs;
	WindowsTargetLibs Win32Libs;
};

struct PlatformLibDef
{
	LinuxTargetLibs Linux64Libs;
	LinuxTargetLibs Linux32Libs;
	WindowsTargetLibs Win64Libs;
	WindowsTargetLibs Win32Libs;
};

enum ModuleType
{
	EXECUTABLE, LIBRARY
};

class ExternDependency
{
public:
	std::string Name;

	std::string GetIncludePath() const;

	std::string GetPlatformLibraryPath(const BuildSettings& Settings) const;
	std::string GetPlatformBinaryPath(const BuildSettings& Settings) const;

	void GetPlatformLibs(const BuildSettings& Settings, std::vector<std::string>& OutLibs) const;
	void GetPlatformBins(const BuildSettings& Settings, std::vector<std::string>& OutBins) const;
	void GetPlatformBinPaths(const BuildSettings& Settings, std::vector<std::string>& OutBins) const;

};

class Module
{
public:

	std::string Name;
	ModuleType Type;
	std::string RootDir;
	std::vector<std::string> ModuleDependencies;
	std::vector<std::string> MacroDefinitions;
	std::vector<ExternDependency> ExternDependencies;

	std::vector<std::string> PythonIncludes;
	std::vector<std::string> PythonLibraryPaths;
	std::vector<std::string> PythonLibraries;

	std::string ModuleFilePath;
	
	// Whether this module built successfully without errors
	bool bBuiltSuccessfully = true;

	bool bNeededFullRebuild = false;

	// Flag used when iterating through models
	bool bVisisted = false;

	// Whether we've attempted to build this module. Note this doesn't mean the build was successful.
	bool bAttemptedBuild = false;

	bool bEngineModule = false;

	bool HasCorrespondingObject(std::string ObjectDir, std::string ObjectExt, std::string SourceFile)
	{
		Filesystem::path SourceFilePath = Filesystem::path(SourceFile);
		
		std::string SourceFileName = SourceFilePath.filename().string();
		SourceFileName = SourceFileName.substr(0, SourceFileName.find(SourceFilePath.extension().string())); // Strip off the file extension

		Filesystem::path ObjFilePath = (Filesystem::path(ObjectDir) / (SourceFileName + ObjectExt)).string();

		return Filesystem::exists(ObjFilePath);
	}
	
	void DiscoverHeaders(std::vector<std::string>& OutHeaders, bool bThirdParty = false) const
	{
		Filesystem::recursive_directory_iterator DirectoryItr(GetIncludeDir());

		for (Filesystem::path File : DirectoryItr)
		{
			// Detect if this file is a source file
			if (File.extension() == ".h")
			{
				OutHeaders.push_back(Filesystem::absolute(File).string());
			}
		}

		// Optionally scan through third party directory for source
		if (bThirdParty)
		{
			Filesystem::recursive_directory_iterator DirectoryItrThirdParty(GetThirdPartyDir());

			for (Filesystem::path File : DirectoryItrThirdParty)
			{
				// Detect if this file is a source file
				if (File.extension() == ".h")
				{
					OutHeaders.push_back(Filesystem::absolute(File).string());
				}
			}
		}
	}

	bool SourceCheck() const
	{
		Filesystem::recursive_directory_iterator DirectoryItr(GetCppDir());

		for (Filesystem::path File : DirectoryItr)
		{
			// Detect if this file is a source file
			if (File.extension() == ".cpp" || File.extension() == ".hpp" || File.extension() == ".c")
			{
				return true;
			}
		}

		return false;
	}
	
	void DiscoverSource(std::vector<std::string>& OutSource, bool bThirdParty = false) const
	{
		Filesystem::recursive_directory_iterator DirectoryItr(GetCppDir());

		for (Filesystem::path File : DirectoryItr)
		{
			// Detect if this file is a source file
			if (File.extension() == ".cpp" || File.extension() == ".hpp" || File.extension() == ".c")
			{
				OutSource.push_back(Filesystem::absolute(File).string());
			}
		}

		// Optionally scan through third party directory for source
		if(bThirdParty)
		{
			Filesystem::recursive_directory_iterator DirectoryItrThirdParty (GetThirdPartyDir());

			for (Filesystem::path File : DirectoryItrThirdParty)
			{
				// Detect if this file is a source file
				if (File.extension() == ".cpp" || File.extension() == ".hpp" || File.extension() == ".c")
				{
					OutSource.push_back(Filesystem::absolute(File).string());
				}
			}
		}
	}

	void DiscoverObjectFiles(std::string ObjectDir, std::string ObjectExt, std::vector<std::string>& OutFoundObjectFiles, std::vector<std::string>& OutMissingObjectFiles)
	{
		std::vector<std::string> DiscoveredSource;
		DiscoverSource(DiscoveredSource);

		for (const std::string& SourceFile : DiscoveredSource)
		{
			Filesystem::path SourceFilePath = Filesystem::path(SourceFile);

			std::string SourceFileName = SourceFilePath.filename().string();
			SourceFileName = SourceFileName.substr(0, SourceFileName.find(SourceFilePath.extension().string())); // Strip off the file extension

			Filesystem::path ObjFilePath = (Filesystem::path(ObjectDir) / (SourceFileName + ObjectExt)).string();

			if (Filesystem::exists(ObjFilePath))
			{
				OutFoundObjectFiles.push_back(ObjFilePath.string());
			}
			else
			{
				OutMissingObjectFiles.push_back(ObjFilePath.string());
			}

		}
	}

	std::string GetRootDir() const
	{
		return Filesystem::absolute(RootDir).string() + (char) Filesystem::path::preferred_separator;
	}

	std::string GetCppDir() const
	{
		return (Filesystem::path(RootDir) / (std::string("Source") + (char)Filesystem::path::preferred_separator)).string();
	}

	std::string GetThirdPartyBinDir() const
	{
		return (Filesystem::path(GetThirdPartyDir()) / (std::string("Binaries") + (char)Filesystem::path::preferred_separator)).string();
	}

	std::string GetThirdPartyDir() const
	{
		return (Filesystem::path(RootDir) / (std::string("ThirdParty") + (char)Filesystem::path::preferred_separator)).string();
	}

	std::string GetIncludeDir() const
	{
		return (Filesystem::path(RootDir) / (std::string("Include") + (char)Filesystem::path::preferred_separator)).string();
	}

	std::string GetGeneratedDir() const;
	
	std::string GetArtifactName() const
	{
		return std::string("RyRuntime-") + Name;
	}

	bool IsExecutable(BuildSettings& Settings) const
	{
		if(Settings.bDistribute && !bEngineModule && Type == ModuleType::EXECUTABLE)
		{
			return true;
		}

		if (!Settings.bDistribute && bEngineModule && Type == ModuleType::EXECUTABLE)
		{
			return true;
		}

		return false;
	}

	void GetTargetBins(const BuildSettings& Settings, std::vector<std::string>& OutBinaries)
	{
		Filesystem::path ThirdPartyBin = GetThirdPartyBinDir();

		if(Settings.TargetPlatform.Arch == TargetArchitecture::x86_64)
		{
			ThirdPartyBin /= "x64";
		}
		else if(Settings.TargetPlatform.Arch == TargetArchitecture::x86)
		{
			ThirdPartyBin /= "x86";
		}
		else if(Settings.TargetPlatform.Arch == TargetArchitecture::Arm)
		{
			ThirdPartyBin /= "Arm";
		}
		else
		{
			return;
		}

		if (Settings.TargetPlatform.OS == TargetOS::Windows)
		{
			ThirdPartyBin /= "Windows";
		}
		else if(Settings.TargetPlatform.OS == TargetOS::Mac)
		{
			ThirdPartyBin /= "Mac";
		}
		else if(Settings.TargetPlatform.OS == TargetOS::Linux)
		{
			ThirdPartyBin /= "Linux";
		}
		else
		{
			return;
		}

		if (Settings.Toolset == BuildToolset::MSVC)
		{
			ThirdPartyBin /= "MSVC";
		}
		else if (Settings.Toolset == BuildToolset::GCC)
		{
			if(Settings.TargetPlatform.OS == TargetOS::Windows)
			{
				ThirdPartyBin /= "MinGW";
			}
			else
			{
				ThirdPartyBin /= "GCC";
			}
		}
		else if (Settings.Toolset == BuildToolset::CLANG)
		{
			ThirdPartyBin /= "Clang";
		}
		else
		{
			return;
		}

		if(Filesystem::exists(ThirdPartyBin))
		{
			Filesystem::directory_iterator BinItr(ThirdPartyBin);

			for (auto& Bin : BinItr)
			{
				OutBinaries.push_back(Bin.path().string());
			}
		}

	}
	
};

Module* LoadModule(Filesystem::path Path);
Module* LoadModulePython(Filesystem::path Path, const BuildSettings* Settings);

void DiscoverModules(Filesystem::path RootDir, std::vector<Module*>& OutModules);
void LoadModules(Filesystem::path RootDir, std::vector<Module*>& OutModules, const BuildSettings* Settings = nullptr);
bool VerifyModules(std::vector<Module*>& OutModules);
bool IsModuleOutOfDate(const Module& Module, std::string BinaryDir, BuildSettings Settings);

void TopSort(const std::vector<std::string>& InModules, const std::map<std::string, Module*>& ModMap, std::vector<std::string>& OutModules);
void TopSort_Helper(std::vector<std::string>& OutModules, const std::map<std::string, Module*>& ModMap, std::set<std::string>& Visited, std::string RootModule);

void RecurseDependencies(const Module& Mod, const std::map<std::string, Module*>& ModMap, std::vector<std::string>& OutMods);