#include "Build/Module/Module.h"
#include <map>
#include <iostream>
#include <fstream>
#include "Common.h"
#include "Util/Util.h"

void LoadPlatformStringArray(std::vector<std::string>& Out, const Json& ParentJson)
{
	if (!ParentJson.is_array())
		return;

	int Count = ParentJson.size();

	for (int Index = 0; Index < Count; Index++)
	{
		if (ParentJson[Index].is_string())
		{
			Out.push_back(ParentJson[Index].get<std::string>());
		}
	}
}

std::string ExternDependency::GetIncludePath() const
{
	return (Filesystem::absolute((Filesystem::path(GetEngineExternPath(Name)) / ("Include")))).string();
}

std::string ExternDependency::GetPlatformLibraryPath(const BuildSettings& Settings) const
{
	return (Filesystem::absolute((Filesystem::path(GetEngineExternPath(Name)) / "Libraries" / Settings.GetTargetPathString()))).string();
}

std::string ExternDependency::GetPlatformBinaryPath(const BuildSettings& Settings) const
{
	return (Filesystem::absolute((Filesystem::path(GetEngineExternPath(Name)) / "Binary" / Settings.GetTargetPathString()))).string();
}

void ExternDependency::GetPlatformLibs(const BuildSettings& Settings, std::vector<std::string>& OutLibs) const
{
	std::string LibPath = GetPlatformLibraryPath(Settings);

	Filesystem::directory_iterator DirItr(LibPath);

	for(auto& Path : DirItr)
	{
		// Libraries must have a name
		if (!Path.path().has_stem())
			continue;

		// It's assumed that the build tool will do the stemming on this string (i.e. libLibrary.a -> Library)
		OutLibs.push_back(Path.path().filename().string());
	}
}

void ExternDependency::GetPlatformBins(const BuildSettings& Settings, std::vector<std::string>& OutBins) const
{
	std::string BinPath = GetPlatformBinaryPath(Settings);

	Filesystem::directory_iterator DirItr(BinPath);

	for (auto& Path : DirItr)
	{
		// Libraries must have a name
		if (!Path.path().has_stem())
			continue;

		// It's assumed that the build tool will do the stemming on this string (i.e. libLibrary.a -> Library)
		OutBins.push_back(Filesystem::absolute(Path.path()).string());
	}
}

Module* LoadModule(Filesystem::path Path)
{
	if(!Filesystem::exists(Path))
	{
		std::cerr << "No module file at " << Path.string() << std::endl;
		return nullptr;
	}
	
	Module* NewModule = new Module;
	NewModule->RootDir = Filesystem::absolute(Path.parent_path()).string();
	NewModule->ModuleFilePath = Filesystem::canonical(Path).string();

	std::ifstream ModuleInFile(Path.string());

	// Check if this module is an engine module.
	// Differentiating engine modules from other modules is useful for splitting up binaries, intermediates, etc
	std::string EngineModulePath = GetEngineModulesDir();
	if(Filesystem::exists(EngineModulePath) && Filesystem::canonical(Path).string().find(Filesystem::canonical(EngineModulePath).string()) == 0)
	{
		NewModule->bEngineModule = true;
	}
	
	try
	{
		Json ModuleJson = Json::parse(ModuleInFile);

		std::string NameField = "Name",
		TypeField = "Type",
		ModulesField = "Modules",
		MacrosField = "Macros",

		ThirdPartyField = "ThirdParty",
		ThirdPartyIncludeField = "Include",
		ThirdPartyLibrariesField = "LibraryPaths",

		// Libraries
		LibrariesField = "Libraries",
		StaticLibrariesField = "Static",
		DynamicLibrariesField = "Dynamic",
		Win64Field = "Win64",
		LinuxField = "Linux";
		

		if (ModuleJson.contains(NameField) && ModuleJson[NameField].is_string())
		{
			ModuleJson[NameField].get_to(NewModule->Name);
		}

		if (ModuleJson.contains(TypeField) && ModuleJson[TypeField].is_string())
		{
			std::string ModuleType;
			ModuleJson[TypeField].get_to(ModuleType);

			if (ModuleType == "Executable")
			{
				NewModule->Type = ModuleType::EXECUTABLE;
			}
			else if (ModuleType == "Runtime")
			{
				NewModule->Type = ModuleType::LIBRARY;
			}
		}

		if (ModuleJson.contains(ModulesField) && ModuleJson[ModulesField].is_array())
		{
			int Count = ModuleJson[ModulesField].size();

			for (int Index = 0; Index < Count; Index++)
			{
				if (ModuleJson[ModulesField][Index].is_string())
				{
					NewModule->ModuleDependencies.push_back(ModuleJson[ModulesField][Index].get<std::string>());
				}
			}
		}

		// Load macro definitions if any
		if (ModuleJson.contains(MacrosField) && ModuleJson[MacrosField].is_array())
		{
			int Count = ModuleJson[MacrosField].size();

			for (int Index = 0; Index < Count; Index++)
			{
				if (ModuleJson[MacrosField][Index].is_string())
				{
					NewModule->MacroDefinitions.push_back(ModuleJson[MacrosField][Index].get<std::string>());
				}
			}
		}

		// Load system libs if any
		if (ModuleJson.contains(LibrariesField))
		{
			Json LibrariesJson = ModuleJson[LibrariesField];

			if(LibrariesJson.contains("x64"))
			{
				Json x64Json = LibrariesJson["x64"];

				if(x64Json.contains("Windows"))
				{
					Json WindowsJson = x64Json["Windows"];


					if(WindowsJson.contains("MSVC"))
					{
						LoadPlatformStringArray(NewModule->Libs.Win64Libs.MSVCLibs, WindowsJson["MSVC"]);
					}

					if (WindowsJson.contains("MinGW"))
					{
						LoadPlatformStringArray(NewModule->Libs.Win64Libs.MinGWLibs, WindowsJson["MinGW"]);
					}

				}

				if (x64Json.contains("Linux"))
				{
					Json LinuxJson = x64Json["Linux"];

					if (LinuxJson.contains("GCC"))
					{
						LoadPlatformStringArray(NewModule->Libs.Linux64Libs.GCCLibs, LinuxJson["GCC"]);
					}
				}

			}
			
		}

		if(ModuleJson.contains("Extern"))
		{
			Json ExternJson = ModuleJson["Extern"];

			// Add engine third party libraries here
			if(ExternJson.is_array())
			{
				for(int Extern = 0; Extern < ExternJson.size(); Extern++)
				{
					ExternDependency NewDep;
					NewDep.Name = ExternJson.at(Extern).get<std::string>();
					NewModule->ExternDependencies.push_back(NewDep);
				}
			}
		}

		if (ModuleJson.contains(ThirdPartyField) && ModuleJson[ThirdPartyField].is_object())
		{
			Json ThirdPartyJson = ModuleJson[ThirdPartyField];
			
			if (ThirdPartyJson.contains(ThirdPartyIncludeField) && ThirdPartyJson[ThirdPartyIncludeField].is_array())
			{
				Json IncludeJson = ThirdPartyJson[ThirdPartyIncludeField];
				
				int IncludePathCount = IncludeJson.size();

				for (int PathIndex = 0; PathIndex < IncludePathCount; PathIndex++)
				{
					if (IncludeJson[PathIndex].is_string())
					{
						NewModule->ModuleThirdParty.Includes.push_back(IncludeJson[PathIndex].get<std::string>());
					}
				}
			}

			if (ThirdPartyJson.contains(ThirdPartyLibrariesField))
			{
				Json LibrariesJson = ThirdPartyJson[ThirdPartyLibrariesField];

				if (LibrariesJson.contains("x64"))
				{
					Json x64Json = LibrariesJson["x64"];

					if (x64Json.contains("Windows"))
					{
						Json WindowsJson = x64Json["Windows"];


						if (WindowsJson.contains("MSVC"))
						{
							LoadPlatformStringArray(NewModule->ModuleThirdParty.Win64Libs.MSVCLibs, WindowsJson["MSVC"]);
						}

						if (WindowsJson.contains("MinGW"))
						{
							LoadPlatformStringArray(NewModule->ModuleThirdParty.Win64Libs.MinGWLibs, WindowsJson["MinGW"]);
						}

					}

					if (x64Json.contains("Linux"))
					{
						Json LinuxJson = x64Json["Linux"];

						if (LinuxJson.contains("GCC"))
						{
							LoadPlatformStringArray(NewModule->ModuleThirdParty.Linux64Libs.GCCLibs, LinuxJson["GCC"]);
						}
					}

				}
			}
		}


	}
	catch (Json::parse_error& Error)
	{
		std::cerr << "Erroring parsing module file " << Path << std::endl;
		return nullptr;
	}

	return NewModule;
}

void DiscoverModules(Filesystem::path RootDir, std::vector<Module*>& OutModules)
{
	Filesystem::path FoundModuleFile;

	Filesystem::directory_iterator NewDirectoryItr(RootDir);
	for (Filesystem::path File : NewDirectoryItr)
	{
		if (File.extension() == ".module")
		{
			// Do not recurse any further 
			FoundModuleFile = File;
			break;
		}
	}

	if (!FoundModuleFile.empty())
	{
		Module* NewModule = LoadModule(FoundModuleFile);

		if (NewModule)
		{
			OutModules.push_back(NewModule);
		}
		else
		{
			return;
		}
	}
	else
	{
		// Recurse into each directory to try and find a module definition
		Filesystem::directory_iterator NewDirectoryItr(RootDir);
		for (Filesystem::path File : NewDirectoryItr)
		{
			// Only go into directories
			if (Filesystem::is_directory(File))
			{
				DiscoverModules(Filesystem::absolute(File), OutModules);
			}
		}
	}
}

bool VerifyModules(std::vector<Module*>& OutModules)
{
	std::map<std::string, int> NameCount;

	for (const Module* Module : OutModules)
	{
		int Current = NameCount[Module->Name] + 1;
		NameCount[Module->Name] = Current;

		// if (NameCount.find(Module->Name) != NameCount.end())
		// {
		// }
		// else
		// {
		// 	NameCount.insert(std::make_pair(Module->Name, 1));
		// }

		if (NameCount[Module->Name] > 1)
		{
			std::cerr << "Module " << Module->Name << " defined more than once!" << std::endl;
			return false;
		}
	}

	return true;
}

bool IsModuleOutOfDate(const Module& Module, std::string BinaryDir, BuildSettings Settings)
{
	// Determine whether this is executable or DLL
	// Note: always rebuild on file error

	std::error_code FileErrorCode;
	std::string ArtifactName = Module.GetArtifactName();

	if (Module.IsExecutable(Settings))
	{
		ArtifactName += ".exe"; // gwindows only
	}
	else
	{
		ArtifactName += ".dll"; // windows only
	}

	std::string ArtifactPath = (Filesystem::path(BinaryDir) / ArtifactName).string();

	Filesystem::file_time_type LastArtifactWriteTime = Filesystem::last_write_time(ArtifactPath, FileErrorCode);

	// Probably means the file did not exist
	if (FileErrorCode)
	{
		return true;
	}

	Filesystem::recursive_directory_iterator DirectoryItr(Module.GetSourceDir());

	for (Filesystem::path File : DirectoryItr)
	{
		if (File.extension() == ".cpp" || File.extension() == ".h")
		{
			Filesystem::file_time_type LastSourceWriteTime = Filesystem::last_write_time(File, FileErrorCode);

			if (FileErrorCode || LastSourceWriteTime >= LastArtifactWriteTime)
			{
				return true;
			}
		}
	}

	return false;
}

void TopSort(const std::vector<std::string>& InModules, const std::map<std::string, Module*>& ModMap, std::vector<std::string>& OutModules)
{
	std::set<std::string> Visited;

	// Top sort all module dependencies
	for (const std::string& Mod : InModules)
	{
		TopSort_Helper(OutModules, ModMap, Visited, Mod);
	}
}

void TopSort_Helper(std::vector<std::string>& OutModules, const std::map<std::string, Module*>& ModMap, std::set<std::string>& Visited, std::string RootModule)
{
	// If we've already visited this module, return
	if (Visited.find(RootModule) != Visited.end())
		return;

	// Ensure we're in the modules map
	if (ModMap.find(RootModule) == ModMap.end())
		return;

	// Mark this module as visited
	Visited.insert(RootModule);

	Module* Mod = ModMap.at(RootModule);

	// Ensure the module is valid
	if (!Mod)
		return;

	for (const std::string Dep : Mod->ModuleDependencies)
	{
		TopSort_Helper(OutModules, ModMap, Visited, Dep);
	}

	// Add ourself
	OutModules.insert(OutModules.begin(), RootModule);
}