#pragma once

#include <string>
#include <vector>
#include "Module/Module.h"
#include <map>
#include <thread>
#include "BuildSettings.h"
#include <unordered_set>

class AbstractBuildTool
{

public:

	AbstractBuildTool(std::string RootPath, BuildSettings Settings)
	{
		this->RootPath = RootPath;
		this->CompileThreadCount = std::thread::hardware_concurrency();
		 // this->CompileThreadCount = 1;

		this->Settings = Settings;

		// Setup build environments
		BinaryDir = Filesystem::absolute(Filesystem::path(RootPath).parent_path() / "Binary").string();
		ObjectDirectory = Filesystem::absolute(Filesystem::path(RootPath).parent_path() / "Intermediate" / "Object").string();
		LibraryDir = Filesystem::absolute(Filesystem::path(RootPath).parent_path() / "Intermediate" / "Libraries").string();

		// BinaryDir += (char)Filesystem::path::preferred_separator;
		// ObjectDirectory += (char)Filesystem::path::preferred_separator;
		// LibraryDir += (char)Filesystem::path::preferred_separator;
	}

	virtual ~AbstractBuildTool()
	{
		
	}

	virtual void CreateGeneratedModuleSource(Module& TheModule);
	
	/**
	 * Instructs the build tool to compile a single source file.
	 *
	 */
	virtual bool BuildSingleSource(std::string ParentModuleName, std::string OutputDirectory, std::string SourceFile, std::string& StdOut, std::string& StdErr);

	/**
	 * Links a module that has been successfully built.
	 *
	 */
	virtual bool LinkModule(std::string ParentModuleName);

	/**
	 * Builds a module by name. The module must have been registered previously with AddModule.
	 * 
	 */
	virtual bool BuildModule(std::string ModuleName);

	/**
	 * Links all built files from BuildProgram together into a single distributable exe.
	 */
	virtual bool LinkStandalone(std::string OutputDirectory, std::string ObjectDirectory, std::string StandaloneName) = 0;

	virtual void AddModule(Module* Module)
	{
		if(Module)
		{
			Modules.insert(std::make_pair(Module->Name, Module));
		}
	}

	virtual void SetCompileThreadCount(unsigned int ThreadCount)
	{
		this->CompileThreadCount = ThreadCount;
	}
	
	/**
	 * Builds modules according to the specified build settings.
	 */
	virtual bool BuildAll(std::vector<std::string>& ModulesFailed);


protected:

	std::string GetModuleBinaryDir(const Module& Mod);
	std::string GetModuleLibraryDir(const Module& Mod);
	std::string GetModuleObjectDir(const Module& Mod);
	
	virtual bool CompileModule(Module& TheModule, std::string OutputDirectory, bool& bNeedsLink);

	virtual bool BuildSingleSource(const Module& TheModule, std::string OutputDirectory, std::string SourceFile, std::string& StdOut, std::string& StdErr) = 0;

	virtual void CopyBinaries(std::string OutputDir);

	/**
	 * Links a module that has been successfully built.
	 *
	 */
	virtual bool LinkModule(Module& TheModule) = 0;

	std::map<std::string, Module*> Modules;

	std::string ObjectFileExtension;

	/**
	 * The amount of threads to use when compiling source files.
	 */
	unsigned int CompileThreadCount;

	void FindOutOfDateSourceFiles(const Module& Module, std::string IntDir, std::vector<std::string>& OutFiles);

	std::string RootPath;

	std::string ObjectDirectory;
	std::string BinaryDir;
	std::string LibraryDir;

	/**
	 * System includes passed into compiler.
	 */
	std::string SystemIncludePath;

	/*
	 * System libraries passed into compiler.
	 */
	std::string SystemLibsPath;

	BuildSettings Settings;

private:

	struct CircularHelper
	{
		CircularHelper* Parent;
		Module* Mod;
	};

	bool CheckDependenies(std::string& ErrorMsg);

	bool CheckCircularHelper(CircularHelper* Helper, std::vector<Module*>& Chain, std::unordered_set<std::string>& Visited, std::vector<CircularHelper*>& Allocated);
	bool CheckCircular(std::vector<Module*>& Chain);

	/**
	 *
	 */
	virtual bool BuildAllModular(std::vector<std::string>& ModulesFailed);

	virtual bool BuildAllStandalone();

};

// Definition for commands
bool BuildCmd(std::vector<std::string>& Args);
bool CleanCmd(std::vector<std::string>& Args);
bool RebuildCmd(std::vector<std::string>& Args);
