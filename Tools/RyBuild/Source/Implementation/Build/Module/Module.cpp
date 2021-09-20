#include "Build/Module/Module.h"
#include <map>
#include <iostream>
#include <fstream>
#include "Common.h"
#include "Util/Util.h"
#include "Python/Python.h"

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

	if (!Filesystem::exists(LibPath))
	{
		return;
	}

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

	if(!Filesystem::exists(BinPath))
	{
		return;
	}

	Filesystem::directory_iterator DirItr(BinPath);

	for (auto& Path : DirItr)
	{
		// Libraries must have a name
		if (!Path.path().has_stem())
			continue;

		// It's assumed that the build tool will do the stemming on this string (i.e. libLibrary.a -> Library)
		OutBins.push_back(Path.path().filename().string());
	}
}

void ExternDependency::GetPlatformBinPaths(const BuildSettings& Settings, std::vector<std::string>& OutBins) const
{
	std::string BinPath = GetPlatformBinaryPath(Settings);

	if (!Filesystem::exists(BinPath))
	{
		return;
	}

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


					// if(WindowsJson.contains("MSVC"))
					// {
					// 	LoadPlatformStringArray(NewModule->Libs.Win64Libs.MSVCLibs, WindowsJson["MSVC"]);
					// }
					//
					// if (WindowsJson.contains("MinGW"))
					// {
					// 	LoadPlatformStringArray(NewModule->Libs.Win64Libs.MinGWLibs, WindowsJson["MinGW"]);
					// }

				}

				// if (x64Json.contains("Linux"))
				// {
				// 	Json LinuxJson = x64Json["Linux"];
				//
				// 	if (LinuxJson.contains("GCC"))
				// 	{
				// 		LoadPlatformStringArray(NewModule->Libs.Linux64Libs.GCCLibs, LinuxJson["GCC"]);
				// 	}
				// }

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

		// if (ModuleJson.contains(ThirdPartyField) && ModuleJson[ThirdPartyField].is_object())
		// {
		// 	Json ThirdPartyJson = ModuleJson[ThirdPartyField];
		// 	
		// 	if (ThirdPartyJson.contains(ThirdPartyIncludeField) && ThirdPartyJson[ThirdPartyIncludeField].is_array())
		// 	{
		// 		Json IncludeJson = ThirdPartyJson[ThirdPartyIncludeField];
		// 		
		// 		int IncludePathCount = IncludeJson.size();
		//
		// 		for (int PathIndex = 0; PathIndex < IncludePathCount; PathIndex++)
		// 		{
		// 			if (IncludeJson[PathIndex].is_string())
		// 			{
		// 				NewModule->ModuleThirdParty.Includes.push_back(IncludeJson[PathIndex].get<std::string>());
		// 			}
		// 		}
		// 	}
		//
		// 	if (ThirdPartyJson.contains(ThirdPartyLibrariesField))
		// 	{
		// 		Json LibrariesJson = ThirdPartyJson[ThirdPartyLibrariesField];
		//
		// 		if (LibrariesJson.contains("x64"))
		// 		{
		// 			Json x64Json = LibrariesJson["x64"];
		//
		// 			if (x64Json.contains("Windows"))
		// 			{
		// 				Json WindowsJson = x64Json["Windows"];
		//
		//
		// 				if (WindowsJson.contains("MSVC"))
		// 				{
		// 					LoadPlatformStringArray(NewModule->ModuleThirdParty.Win64Libs.MSVCLibs, WindowsJson["MSVC"]);
		// 				}
		//
		// 				if (WindowsJson.contains("MinGW"))
		// 				{
		// 					LoadPlatformStringArray(NewModule->ModuleThirdParty.Win64Libs.MinGWLibs, WindowsJson["MinGW"]);
		// 				}
		//
		// 			}
		//
		// 			if (x64Json.contains("Linux"))
		// 			{
		// 				Json LinuxJson = x64Json["Linux"];
		//
		// 				if (LinuxJson.contains("GCC"))
		// 				{
		// 					LoadPlatformStringArray(NewModule->ModuleThirdParty.Linux64Libs.GCCLibs, LinuxJson["GCC"]);
		// 				}
		// 			}
		//
		// 		}
		// 	}
		// }


	}
	catch (Json::parse_error& Error)
	{
		std::cerr << "Erroring parsing module file " << Path << std::endl;
		return nullptr;
	}

	return NewModule;
}

bool LoadPythonStringList(std::vector<std::string>& Out, PyObject* List, std::string NullErr, std::string NotStringListErr, std::string ElementNotStringErr)
{
	if(!List)
	{
		if(!NullErr.empty())
		{
			std::cerr << NullErr << std::endl;
			return false;
		}

		return true;
	}
	
	if (!PyList_Check(List))
	{
		std::cerr << NotStringListErr << std::endl;
		return false;
	}

	Py_ssize_t ListSize = PyList_Size(List);

	for (Py_ssize_t Index = 0; Index < ListSize; Index++)
	{
		PyObject* PyModuleDependency = PyList_GetItem(List, Index);

		if (PyUnicode_Check(PyModuleDependency))
		{
			PyObject* Utf8 = PyUnicode_AsEncodedString(PyModuleDependency, "utf-8", "~E~");
			const char* Bytes = PyBytes_AsString(Utf8);

			Out.push_back(Bytes);

			Py_DECREF(Utf8);
		}
		else
		{
			std::cerr << ElementNotStringErr << std::endl;
			return false;
		}
	}

	return true;
}

Module* DiscoverModule(Filesystem::path Path)
{
	if (!Filesystem::exists(Path))
	{
		std::cerr << "No python module build file at " << Path.string() << std::endl;
		return nullptr;
	}

	Module* NewModule = new Module;
	NewModule->Name = Path.stem().string();
	NewModule->RootDir = Filesystem::absolute(Path.parent_path()).string();
	NewModule->ModuleFilePath = Filesystem::canonical(Path).string();

	// Check if this module is an engine module.
	// Differentiating engine modules from other modules is useful for splitting up binaries, intermediates, etc
	std::string EngineModulePath = GetEngineModulesDir();
	if (Filesystem::exists(EngineModulePath) && Filesystem::canonical(Path).string().find(Filesystem::canonical(EngineModulePath).string()) == 0)
	{
		NewModule->bEngineModule = true;
	}

	// Cut off the .build part
	int FirstDot = NewModule->Name.find_first_of('.');
	if (FirstDot != std::string::npos)
	{
		NewModule->Name = NewModule->Name.substr(0, FirstDot);
	}

	std::vector<std::string> ExternDeps;

	// Create external dependencies
	for (const std::string& ExternDep : ExternDeps)
	{
		ExternDependency NewDep;
		NewDep.Name = ExternDep;
		NewModule->ExternDependencies.push_back(NewDep);
	}

	return NewModule;
}

Module* LoadModulePython(Filesystem::path Path, const BuildSettings* Settings)
{
	if (!Filesystem::exists(Path))
	{
		std::cerr << "No python module build file at " << Path.string() << std::endl;
		return nullptr;
	}
	
	// Load module python
	std::string ModulePython = "";
	std::string NextLine = "";
	std::ifstream ModuleInFile(Path.string());
	while(std::getline(ModuleInFile, NextLine))
	{
		ModulePython += NextLine + "\n";
	}

	Module* NewModule = new Module;
	NewModule->Name = Path.stem().string();
	NewModule->RootDir = Filesystem::absolute(Path.parent_path()).string();
	NewModule->ModuleFilePath = Filesystem::canonical(Path).string();

	// Check if this module is an engine module.
	// Differentiating engine modules from other modules is useful for splitting up binaries, intermediates, etc
	std::string EngineModulePath = GetEngineModulesDir();
	if (Filesystem::exists(EngineModulePath) && Filesystem::canonical(Path).string().find(Filesystem::canonical(EngineModulePath).string()) == 0)
	{
		NewModule->bEngineModule = true;
	}

	// Cut off the .build part
	int FirstDot = NewModule->Name.find_first_of('.');
	if(FirstDot != std::string::npos)
	{
		NewModule->Name = NewModule->Name.substr(0, FirstDot);
	}

	// Set system path for python libs
	Filesystem::path LibsPath = Filesystem::absolute(Filesystem::path(GetModulePath()).parent_path() / "PythonLib");
	Py_SetPath(LibsPath.wstring().c_str());

	// Initialize python
	Py_Initialize();

	PyObject* Globals = PyModule_GetDict(PyImport_AddModule("__main__"));
	PyObject* Locals = PyDict_New();

	// Variables that can be set by the script
	PyObject* DefModulesList = PyList_New(0);
	PyObject* DefType = PyUnicode_New(0, 10);
	PyObject* DefExterns = PyList_New(0);
	PyObject* DefMacroDefs = PyList_New(0);
	PyObject* DefIncludes = PyList_New(0);
	PyObject* DefLibraryPaths = PyList_New(0);
	PyObject* DefLibraries = PyList_New(0);

	// Variables that are read by the script
	PyObject* TargetArch = nullptr;
	PyObject* TargetOS = nullptr;
	PyObject* BuildToolset = nullptr;
	PyObject* BuildConfig = nullptr;
	PyObject* BuildType = nullptr;
	PyObject* Distribute = nullptr;

	if(Settings)
	{
		TargetArch = PyUnicode_DecodeFSDefault(Settings->TargetArchToString().c_str());
		TargetOS = PyUnicode_DecodeFSDefault(Settings->TargetOSToString().c_str());
		BuildToolset = PyUnicode_DecodeFSDefault(Settings->ToolsetToString().c_str());
		BuildConfig = PyUnicode_DecodeFSDefault(Settings->ConfigToString().c_str());
		BuildType = PyUnicode_DecodeFSDefault(Settings->BuildTypeToString().c_str());
		Distribute = PyBool_FromLong(Settings->bDistribute);
	}
	else
	{
		TargetArch = PyUnicode_DecodeFSDefault("None");
		TargetOS = PyUnicode_DecodeFSDefault("None");
		BuildToolset = PyUnicode_DecodeFSDefault("None");
		BuildConfig = PyUnicode_DecodeFSDefault("None");
		BuildType = PyUnicode_DecodeFSDefault("None");
		Distribute = PyBool_FromLong(0);

		std::cout << "Warning: Build settings not passed in" << std::endl;
	}

	// Set global inputs
	PyDict_SetItemString(Locals, "Modules", DefModulesList);
	PyDict_SetItemString(Locals, "Type", DefType);
	PyDict_SetItemString(Locals, "Extern", DefExterns);
	PyDict_SetItemString(Locals, "MacroDefs", DefMacroDefs);
	PyDict_SetItemString(Locals, "Includes", DefIncludes);
	PyDict_SetItemString(Locals, "LibraryPaths", DefLibraryPaths);
	PyDict_SetItemString(Locals, "Libraries", DefLibraries);

	// Set global constants
	PyDict_SetItemString(Locals, "TargetArch", TargetArch);
	PyDict_SetItemString(Locals, "TargetOS", TargetOS);
	PyDict_SetItemString(Locals, "BuildToolset", BuildToolset);
	PyDict_SetItemString(Locals, "BuildConfig", BuildConfig);
	PyDict_SetItemString(Locals, "BuildType", BuildType);
	PyDict_SetItemString(Locals, "Distribute", Distribute);

	PyObject* RetVal = PyRun_StringFlags(ModulePython.c_str(), Py_file_input, Globals, Locals, nullptr);

	if(PyErr_Occurred())
	{
		std::cerr << "Error parsing build script file " << NewModule->Name << " in " << NewModule->RootDir << ": " << std::endl;
		
		PyErr_Print();
		return nullptr;
	}

	// Extract module list from globals

	// Todo: add smart "FindLib" builtin function

	PyObject* ModulesList = PyDict_GetItemString(Locals, "Modules");
	PyObject* Type = PyDict_GetItemString(Locals, "Type");
	PyObject* Externs = PyDict_GetItemString(Locals, "Extern");
	PyObject* MacroDefs = PyDict_GetItemString(Locals, "MacroDefs");
	PyObject* Includes = PyDict_GetItemString(Locals, "Includes");
	PyObject* LibraryPaths = PyDict_GetItemString(Locals, "LibraryPaths");
	PyObject* Libraries = PyDict_GetItemString(Locals, "Libraries");

	std::vector<std::string> ExternDeps;

	// Load all string lists
	LoadPythonStringList(NewModule->ModuleDependencies, ModulesList, "", "Module dependencies must be list", "All module dependencies must be strings");
	LoadPythonStringList(ExternDeps, Externs, "", "External dependencies must be list", "All external dependencies must be strings");
	LoadPythonStringList(NewModule->MacroDefinitions, MacroDefs, "", "Macro definitions must be a list", "All macro definitions must be strings");
	LoadPythonStringList(NewModule->PythonIncludes, Includes, "", "Include directories must be a list", "All include directories must be strings");
	LoadPythonStringList(NewModule->PythonLibraryPaths, LibraryPaths, "", "Library paths must be a list", "All library paths must be strings");
	LoadPythonStringList(NewModule->PythonLibraries, Libraries, "", "Libraries must be a list", "All libraries must be strings");

	// Create external dependencies
	for(const std::string& ExternDep : ExternDeps)
	{
		ExternDependency NewDep;
		NewDep.Name = ExternDep;
		NewModule->ExternDependencies.push_back(NewDep);
	}

	// int Size = PyDict_Size(Locals);
	// PyObject* Rep = PyObject_Repr(Locals);
	// PyObject* Utf8 = PyUnicode_AsEncodedString(Rep, "utf-8", "~E~");
	// std::string ModType = PyBytes_AsString(Utf8);
	// std::cout << ModType << std::endl;
	// std::cout << Size << std::endl;

	// Load module type
	if(Type)
	{
		if (PyUnicode_Check(Type))
		{
			PyObject* Utf8 = PyUnicode_AsEncodedString(Type, "utf-8", "~E~");
			std::string ModType = PyBytes_AsString(Utf8);

			if(ModType == "Runtime")
			{
				NewModule->Type = LIBRARY;
			}
			else if(ModType == "Executable")
			{
				NewModule->Type = EXECUTABLE;
			}
			else
			{
				std::cerr << "Type must be either Runtime or Executable" << std::endl;
				return nullptr;
			}

			Py_DECREF(Utf8);
		}
		else
		{
			std::cerr << "Module type was not a string" << std::endl;
			return nullptr;
		}
	}
	else
	{
		std::cerr << "Module type was not set: must be either Runtime or Executable" << std::endl;
		return nullptr;
	}

	Py_Finalize();

	return NewModule;
}

void DiscoverModules(Filesystem::path RootDir, std::vector<Module*>& OutModules)
{
	Filesystem::path FoundModuleFile;
	Filesystem::directory_iterator NewDirectoryItr(RootDir);

	for (Filesystem::path File : NewDirectoryItr)
	{
		if (File.extension() == ".py" && File.stem().string().find(".build") != std::string::npos)
		{
			// Do not recurse any further 
			FoundModuleFile = File;
			break;
		}

	}

	if (!FoundModuleFile.empty())
	{
		Module* NewModule = nullptr;

		NewModule = DiscoverModule(FoundModuleFile);

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

void LoadModules(Filesystem::path RootDir, std::vector<Module*>& OutModules, const BuildSettings* Settings)
{
	Filesystem::path FoundModuleFile;
	Filesystem::directory_iterator NewDirectoryItr(RootDir);

	bool bPythonModule = false;
	
	for (Filesystem::path File : NewDirectoryItr)
	{
		if (File.extension() == ".module")
		{
			// Do not recurse any further 
			FoundModuleFile = File;
			break;
		}

		if (File.extension() == ".py")
		{
			// Do not recurse any further 
			FoundModuleFile = File;
			bPythonModule = true;
			break;
		}

	}

	if (!FoundModuleFile.empty())
	{
		Module* NewModule = nullptr;

		if(bPythonModule)
		{
			NewModule = LoadModulePython(FoundModuleFile, Settings);
		}
		else
		{
			NewModule = LoadModule(FoundModuleFile);
		}

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
				LoadModules(Filesystem::absolute(File), OutModules, Settings);
			}
		}
	}
}

bool VerifyModules(std::vector<Module*>& OutModules)
{
	std::map<std::string, int> NameCount;

	for (const Module* Module : OutModules)
	{
		if (!Module)
			continue;
		
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

	Filesystem::recursive_directory_iterator DirectoryItr(Module.GetCppDir());

	for (Filesystem::path File : DirectoryItr)
	{
		if (File.extension() == ".cpp" || File.extension() == ".h" || File.extension() == ".c" || File.extension() == ".hpp")
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

void RecurseDependencies(const Module& Mod, const std::map<std::string, Module*>& ModMap, std::vector<std::string>& OutMods)
{
	std::set<std::string> HitModules;
	std::vector<std::string> ModFrontier;

	for(const std::string& ModDep : Mod.ModuleDependencies)
	{
		ModFrontier.push_back(ModDep);
	}

	while(!ModFrontier.empty())
	{
		std::string Next = ModFrontier[0];
		ModFrontier.erase(ModFrontier.begin());

		// Add this module to the output list
		HitModules.insert(Next);
		
		if(ModMap.find(Next) != ModMap.end())
		{
			Module* MappedMod = ModMap.at(Next);

			if (!MappedMod)
				continue;

			// Add all of this module's dependencies
			for(const std::string ModDep : MappedMod->ModuleDependencies)
			{
				ModFrontier.push_back(ModDep);
			}
		}	
	}

	for(std::string HitMod : HitModules)
	{
		OutMods.push_back(HitMod);
	}	
	
}

std::string Module::GetGeneratedDir() const
{
	return (Filesystem::path(GetEngineIntermediateDir()) / "Generated" / (Name + (char)Filesystem::path::preferred_separator)).string();
}