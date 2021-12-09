#include "Build/BuildTool.h"
#include "Common.h"
#include <map>
#include <iostream>
#include <queue>
#include <thread>
#include <string>
#include <mutex>
#include <chrono>
#include <fstream>
#include <cstdarg>
#include <Json/json.hpp>
#include <sstream>
#include "Util/Util.h"
#include "Build/MSVCBuildTool.h"
#include "Build/GCCBuildTool.h"

void AbstractBuildTool::CreateBaseModuleSource(const Module& TheModule)
{
	constexpr int BUFFER_SIZE = 1024 * 5;
	char GeneratedCodeBuffer[BUFFER_SIZE];

	std::string GeneratedSourceFormat = R"(#pragma once

#if defined(_WIN32)
	
	#if defined(RYBUILD_STANDALONE)
		#define %s_MODULE // No DLL imports
	#elif defined(COMPILE_MODULE_%s)
		#define %s_MODULE __declspec(dllexport) // Export symbols
	#else
		#define %s_MODULE __declspec(dllimport) // Importing from another library
	#endif

#elif defined(__GNUC__) || defined(__MINGW32__)

	#if defined(RYBUILD_STANDALONE) 
		#define %s_MODULE // No shared lib imports
	#elif defined(COMPILE_MODULE_%s)
		#define %s_MODULE __attribute__((visibility("default"))) // Export symbols
	#else
		#define %s_MODULE // Linux does not have shared lib imports
	#endif
	
#elif defined(__APPLE__)
	// Not supported
#elif defined(__unix)
	// Not supported
#endif)";

	std::string ModuleNameCaps = ToUpper(TheModule.Name);

	FormatString(GeneratedCodeBuffer, BUFFER_SIZE, GeneratedSourceFormat.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str(),
		ModuleNameCaps.c_str());

	Filesystem::create_directories(TheModule.GetGeneratedDir());
	std::ofstream ModuleSourceOut(TheModule.GetGeneratedSourcePath());
	{
		ModuleSourceOut << GeneratedCodeBuffer << std::endl;
	}
	ModuleSourceOut.close();
}

bool AbstractBuildTool::CreateGeneratedModuleSource()
{

	struct ModuleData
	{
		std::string ModuleNameCaps;
		std::string GeneratedDirectory;
		std::string GeneratedSourceDirectory;
		std::string IncludeDir;
		std::string SourceDir;
		
		std::vector<std::string> ModuleIncludes;

	};

	std::vector<std::string>* AllHeaders = new std::vector<std::string>;
	std::unordered_map<std::string, ModuleData*>* HeadersToMod = new std::unordered_map<std::string, ModuleData*>;
	std::vector<std::string>* GenPathsNew = new std::vector<std::string>;
	std::vector<std::string>* GenPathsTmp = new std::vector<std::string>;
	std::vector<std::string>* GenPathsOld = new std::vector<std::string>;

	for(auto& TheModule : Modules)
	{
		Module* Mod = TheModule.second;
		ModuleData* Data = new ModuleData;
		
		Data->ModuleNameCaps = ToUpper(TheModule.first);
		
		CreateBaseModuleSource(*TheModule.second);

		Mod->GatherIncludes(Modules, Data->ModuleIncludes);

		Data->GeneratedDirectory = Mod->GetGeneratedDir();
		Data->GeneratedSourceDirectory = Mod->GetGeneratedSourcePath();

		Data->IncludeDir = Mod->GetIncludeDir();
		Data->SourceDir = Mod->GetCppDir();

		std::vector<std::string> Headers;
		FindOutOfDateHeaders(*Mod, Data->GeneratedDirectory, Headers);

		for(auto& Header : Headers)
		{
			AllHeaders->push_back(Header);
			HeadersToMod->insert(std::make_pair(Header, Data));
		}
		
	}

	// Pre-create all .gen.h files for headers that need to be re-created
	for (auto& Header : *AllHeaders)
	{
		ModuleData* Data = HeadersToMod->at(Header);
		
		Filesystem::path HeaderPath = Header;
		std::string HeaderStem = HeaderPath.stem().string();

		std::string GenPathNew = HeaderStem + ".gen.h";
		Filesystem::path FullGenPathNew = Filesystem::path(Data->GeneratedDirectory) / GenPathNew;

		std::string GenPathTmp = HeaderStem + ".gen.tmp.h";
		Filesystem::path FullGenPathTmp = Filesystem::path(Data->GeneratedDirectory) / GenPathTmp;

		std::string GenPathOld = HeaderStem + ".gen.old.h";
		Filesystem::path FullGenPathOld = Filesystem::path(Data->GeneratedDirectory) / GenPathOld;

		GenPathsNew->push_back(FullGenPathNew.string());
		GenPathsTmp->push_back(FullGenPathTmp.string());
		GenPathsOld->push_back(FullGenPathOld.string());

		// Rename current file to prevent changing the file last modified time, need this file in case the code generation fails
		if(Filesystem::exists(FullGenPathNew))
		{
			Filesystem::rename(FullGenPathNew, FullGenPathOld);
		}
		
		// Create simple generated file so clang gets the proper includes/macros to start
		std::ofstream Tap;
		Tap.open(FullGenPathNew.string());
		Tap << "#include \"Core/Reflection.h\"" << std::endl;
		Tap << "#include \"" + Filesystem::path(Data->GeneratedSourceDirectory).filename().string() + "\"\n";
		Tap.close();
	}

	std::mutex HeaderAcquisitionLock;
	std::mutex OutWriteLock;
	std::vector<std::thread*> ThreadPool;
	unsigned int* CurrentHeaderIndex = new unsigned int(0);
	unsigned int* CompletionIndex = new unsigned int(0);
	bool bGenerationSuccess = true;
	std::unordered_map<int32_t, bool>* GenerationResult = new std::unordered_map<int32_t, bool>;

	for (unsigned int ThreadSpawn = 0; ThreadSpawn < CompileThreadCount && ThreadSpawn < HeadersToMod->size(); ThreadSpawn++)
	{
		std::thread* NewThread = new std::thread([this, 
			HeadersToMod,
			AllHeaders,
			GenerationResult,
			&OutWriteLock,
			&bGenerationSuccess,
			&HeaderAcquisitionLock,
			CompletionIndex, 
			CurrentHeaderIndex
		]()
		{
			int32_t RetrievedIndex = 0;
			while (RetrievedIndex >= 0)
			{
				std::string HeaderNeedingGeneration;
				ModuleData* Data;

				// Try to get the next available header
				HeaderAcquisitionLock.lock();
				{
					if (*CurrentHeaderIndex < AllHeaders->size())
					{
						RetrievedIndex = *CurrentHeaderIndex;
						HeaderNeedingGeneration = (*AllHeaders)[*CurrentHeaderIndex];
						(*CurrentHeaderIndex)++;

						Data = HeadersToMod->at(HeaderNeedingGeneration);
					}
					else
					{
						RetrievedIndex = -1;
					}
				}
				HeaderAcquisitionLock.unlock();

				if(RetrievedIndex >= 0)
				{
					Filesystem::path HeaderPath = HeaderNeedingGeneration;
					std::string HeaderStem = HeaderPath.stem().string();
					auto RelativeToInclude = PathRelativeTo(Data->IncludeDir, HeaderPath);
					auto SourceLoc = Data->SourceDir / RelativeToInclude.parent_path() / (HeaderStem + ".cpp");

					std::string HeaderName = HeaderStem + ".gen.h";
					Filesystem::path GenPath = Filesystem::path(Data->GeneratedDirectory) / HeaderName;
					std::string GenHeaderNameTmp = HeaderStem + ".gen.tmp.h";
					Filesystem::path GenPathTmp = Filesystem::path(Data->GeneratedDirectory) / GenHeaderNameTmp;

					if (!Filesystem::exists(SourceLoc)) // If source doesn't exist, assume header only file
					{
						if (HeaderPath.extension() == ".hpp")
						{
							SourceLoc = HeaderPath;
						}
						else
						{
							// TODO: create a stub .cpp file so we can still generate source for this header
							OutWriteLock.lock();
							{
								GenerationResult->insert(std::make_pair(RetrievedIndex, true));
							}
							OutWriteLock.unlock();

							continue;
						}
					}

					// Build arguments to pass into reflection code generator
					std::string RyReflectPath = GetRyReflectPath();
					std::vector<std::string> Args;
					Args.push_back(SourceLoc.string());
					Args.push_back(HeaderPath.filename().string());
					Args.push_back(GenPathTmp.string());

					// Add include paths
					for (const std::string& ModInc : Data->ModuleIncludes)
						Args.push_back("-Include=" + ModInc);

					// Add defines
					Args.push_back("-Define=COMPILE_MODULE_" + Data->ModuleNameCaps);
					Args.push_back("-Define=RBUILD_TARGET_OS_" + ToUpper(OSToString(Settings.TargetPlatform.OS)));

					int StdOutSize = 1024 * 1000;
					char* StdOutBuffer = new char[StdOutSize];
					char* StdErrBuffer = new char[StdOutSize];

					bool bResult = ExecProc(RyReflectPath, Args, StdOutSize, StdOutBuffer, StdOutSize, StdErrBuffer);
					std::string StdOutAsString = StdOutBuffer;
					std::string StdErrAsString = StdErrBuffer;

					// Mutex output writing so they don't stomp on each other
					OutWriteLock.lock();
					{
						// Insert the result of this module file
						GenerationResult->insert(std::make_pair(RetrievedIndex, bResult));

						// Print out header that source was just generated for
						std::cout << "\t[" << (*CompletionIndex + 1) << " of " << AllHeaders->size() << "] " << HeaderPath.filename().string();
						(*CompletionIndex)++;
						if(!bResult)
							std::cout << " [fail]: ";
						else if(!StdOutAsString.empty() || !StdErrAsString.empty())
							std::cout << ": " << std::endl;						
						
						if (!StdOutAsString.empty())
							std::cout << StdOutBuffer;
						if (!StdErrAsString.empty())
							std::cout << StdErrAsString;
							
						std::cout << std::endl;
					}
					OutWriteLock.unlock();

					if(!bResult)
					{
						bGenerationSuccess = false;
					}
				}

			}
		});

		ThreadPool.push_back(NewThread);
	}

	// Cleanup threads
	for (std::thread* Thread : ThreadPool)
	{
		Thread->join();
		delete Thread;
	}

	// Final output to file
	for (int32_t GenIndex = 0; GenIndex < AllHeaders->size(); GenIndex++)
	{
		std::string Header = AllHeaders->at(GenIndex);
		ModuleData* Dat = HeadersToMod->at(Header);

		std::string GenPathNew = GenPathsNew->at(GenIndex);
		std::string GenPathTmp = GenPathsTmp->at(GenIndex);
		std::string GenPathOld = GenPathsOld->at(GenIndex);

		bool bSuccess = GenerationResult->at(GenIndex);

		if (bSuccess)
		{
			// Replace GenNew with GenTmp, delete GenOld
			Filesystem::remove(GenPathOld);

			if (Filesystem::exists(GenPathTmp))
			{
				std::string NextLine = "";
				std::ostringstream OutString;
				std::ifstream FileIn(GenPathTmp);
				while (std::getline(FileIn, NextLine))
				{
					OutString << NextLine << "\n";
				}
				FileIn.close();

				std::string SourcePrefix = "#include \"" + Filesystem::path(Dat->GeneratedSourceDirectory).filename().string() + "\"\n";
				std::string FinalSource = SourcePrefix + OutString.str();

				Filesystem::remove(GenPathTmp);

				std::ofstream OutStream(GenPathNew);
				{
					OutStream << FinalSource;
				}
				OutStream.close();
			}
		}
		else
		{
			// Delete GenPathNew and GenPathTmp, restore GenPathOld
			Filesystem::remove(GenPathTmp);
			Filesystem::remove(GenPathNew);

			if(Filesystem::exists(GenPathOld))
			{
				Filesystem::rename(GenPathOld, GenPathNew);
			}
		}

	}

	// Cleanup multi-threading variables
	delete CurrentHeaderIndex;
	delete CompletionIndex;
	delete AllHeaders;
	delete HeadersToMod;
	delete GenerationResult;

	return bGenerationSuccess;
}

void AbstractBuildTool::FindOutOfDateHeaders(const Module& Module, std::string GeneratedDir, std::vector<std::string>& OutFiles)
{
	std::error_code FileErrorCode;

	// These are files that we need to check against ALL obj timestamps. 
	std::vector<std::string> CheckAgainstAllStamps;

	Filesystem::recursive_directory_iterator IncludeDirectoryItr(Module.GetIncludeDir());

	for(auto Itr : IncludeDirectoryItr)
	{
		auto File = Itr.path();

		bool bIsHeader = File.extension() == ".hpp" || File.extension() == ".h";

		if (bIsHeader)
		{
			// Get the filename without the extension
			std::string FileStem = File.stem().string();

			// Find the corresponding .gen.h file
			std::string GeneratedPath = (Filesystem::path(GeneratedDir) / (FileStem + ".gen.h")).string();

			// Figure out when that obj file was last written to
			Filesystem::file_time_type LastGenWriteTime = Filesystem::last_write_time(GeneratedPath, FileErrorCode);

			// Figure out when this source was last written to
			Filesystem::file_time_type LastHeaderWriteTime = Filesystem::last_write_time(File, FileErrorCode);

			// Header only file, we need to check against all obj timestamps later.
			if (!Filesystem::exists(GeneratedPath) )
			{
				// Generated header didn't exist
				OutFiles.push_back(File.string());
			} else if (FileErrorCode || LastHeaderWriteTime >= LastGenWriteTime)
			{
				OutFiles.push_back(File.string());
			}

		}
	}


}

void AbstractBuildTool::FindOutOfDateSourceFiles(const Module& Module, std::string IntDir, std::vector<std::string>& OutFiles, bool& bHeaderChanged)
{
	// Determine whether this is executable or DLL
	// Note: always rebuild on file error

	std::error_code FileErrorCode;

	// Probably means the file did not exist
	if (FileErrorCode)
	{
		return;
	}

	//bHeaderChanged = false;

	std::vector<std::string> AllSourceFiles;
	std::vector<std::string> OutOfDateFiles;

	// These are files that we need to check against ALL obj timestamps. 
	std::vector<std::string> CheckAgainstAllStamps;

	Filesystem::file_time_type EarliestChangedObj = Filesystem::file_time_type::max();

	Filesystem::recursive_directory_iterator SourceDirectoryItr(Module.GetCppDir());
	Filesystem::recursive_directory_iterator IncludeDirectoryItr(Module.GetIncludeDir());

	auto ProcessFile = [this,
		IntDir,
		&bHeaderChanged,
		&FileErrorCode,
		&CheckAgainstAllStamps,
		&EarliestChangedObj,
		&OutOfDateFiles,
		&AllSourceFiles]
		(const Filesystem::path& File)
	{
		bool bIsCPP = File.extension() == ".cpp" || File.extension() == ".hpp" || File.extension() == ".c";
		bool bIsHeader = File.extension() == ".h";

		if (bIsCPP || bIsHeader)
		{
			// Get the filename without the extension
			std::string FileName = File.filename().string();
			FileName = FileName.substr(0, FileName.find(File.extension().string()));

			// Find the corresponding .obj/.o file
			std::string IntermediatePath = (Filesystem::path(IntDir) / (FileName + ObjectFileExtension)).string();

			// Figure out when that obj file was last written to
			Filesystem::file_time_type LastObjWriteTime = Filesystem::last_write_time(IntermediatePath, FileErrorCode);

			// Figure out when this source was last written to
			Filesystem::file_time_type LastSourceWriteTime = Filesystem::last_write_time(File, FileErrorCode);

			// Header only file, we need to check against all obj timestamps later.
			if (!Filesystem::exists(IntermediatePath) && bIsHeader)
			{
				// This is a header without an associated CPP, so we won't have an OBJ file to compare against.
				// In these cases, we have to compare against the earliest OBJ timestamp to check for recompilation.

				CheckAgainstAllStamps.push_back(File.string());
				return;

				/*if (LastSourceWriteTime < EarliestChangedObj)
				{
					bHeaderWasOutOfDate = true;
					break;
				}
				else
				{
					// We aren't off the hook as we have to check against the remainder of OBJs.
					continue;
				}*/
			}

			if (LastObjWriteTime < EarliestChangedObj)
			{
				EarliestChangedObj = LastObjWriteTime;
			}

			if (FileErrorCode || LastSourceWriteTime >= LastObjWriteTime)
			{
				if (bIsHeader)
				{
					// We're a header, mark EVERYTHING as out of date
					bHeaderChanged = true;
					return;
				}
				else
				{
					// We're a CPP
					OutOfDateFiles.push_back(File.string());
				}
			}

			if (bIsCPP)
			{
				AllSourceFiles.push_back(File.string());
			}

		}
	};

	for (Filesystem::path File : SourceDirectoryItr)
	{
		ProcessFile(File);
	}

	for (Filesystem::path File : IncludeDirectoryItr)
	{
		ProcessFile(File);
	}

	// No point in doing this computation if we already know we need to rebuild all
	if (!bHeaderChanged)
	{
		// Iterate through the check against list to see if a full recompilation is required
		for (const std::string& CheckAgainstAllFile : CheckAgainstAllStamps)
		{
			Filesystem::file_time_type LastWriteTime = Filesystem::last_write_time(CheckAgainstAllFile, FileErrorCode);

			if (LastWriteTime > EarliestChangedObj)
			{
				bHeaderChanged = true;
				break;
			}
		}
	}

	// Add either everything (header was out of date), or only out of date source files
	std::vector<std::string>& VectorToUse = bHeaderChanged ? AllSourceFiles : OutOfDateFiles;
	for (const std::string& SourceFile : VectorToUse)
	{
		OutFiles.push_back(SourceFile);
	}

}

void AbstractBuildTool::CopyBinaries(std::string OutputDir)
{
	for (auto& Mod : Modules)
	{
		std::vector<std::string> OutBins;

		Module* Module = Mod.second;

		// Copy all external dependency binaries
		for (auto& ExternDep : Module->ExternDependencies)
		{
			ExternDep.GetPlatformBinPaths(Settings, OutBins);
		}

		// Copy all module specific binaries
		Module->GetTargetBins(Settings, OutBins);

		for (const std::string& OutBin : OutBins)
		{
			Filesystem::path ToPath= OutputDir / Filesystem::path(OutBin).filename();
			if(!Filesystem::exists(ToPath))
			{
				Filesystem::copy_file(OutBin, ToPath);
			}
		}
	}
}

bool AbstractBuildTool::BuildSingleSource(std::string ParentModuleName, std::string OutputDirectory, std::string SourceFile, std::string& StdOut, std::string& StdErr)
{
	if (Modules.find(ParentModuleName) == Modules.end())
	{
		std::cerr << "Module " << ParentModuleName << " not added with AddModule(). Cannot build." << std::endl;
		return false;
	}

	return BuildSingleSource(*Modules[ParentModuleName], OutputDirectory, SourceFile, StdOut, StdErr);
}

bool AbstractBuildTool::LinkModule(std::string ParentModuleName)
{
	if (Modules.find(ParentModuleName) == Modules.end())
	{
		std::cerr << "Module " << ParentModuleName << " not added with AddModule(). Cannot build." << std::endl;
		return false;
	}

	return LinkModule(*Modules[ParentModuleName]);
}

bool AbstractBuildTool::CompileModule(Module& TheModule, std::string OutputDirectory, bool& bNeedsLink, bool& bNeedsFullRebuild)
{
	// Make sure there's at least one cpp in this module
	if(!TheModule.SourceCheck())
	{
		std::cerr << "Module failed: must have at least one CPP file" << std::endl;
		return false;
	}

	// std::string ModuleBinaryDir = GetModuleBinaryDir(TheModule);
	// std::string ModuleObjectDir = GetModuleObjectDir(TheModule);
	// std::string ModuleLibraryDir = GetModuleLibraryDir(TheModule);
	
	std::mutex SourceAcquisitionLock;
	std::mutex OutWriteLock;
	std::vector<std::string>* SourcesNeedingBuild = new std::vector<std::string>;
	unsigned int* CurrentSourceIndex = new unsigned int(0);
	unsigned int* CompletionIndex = new unsigned int(0);
	std::vector<std::thread*> ThreadPool;

	// Locate source files that need to be built
	FindOutOfDateSourceFiles(TheModule, OutputDirectory, *SourcesNeedingBuild, bNeedsFullRebuild);

	if((*SourcesNeedingBuild).size() <= 0)
	{
		// Don't do any work
		TheModule.bBuiltSuccessfully = true; // Indicate the module built successfully
		bNeedsLink = false; // Indicate that we don't need to re-link this module
		return true; // Indicate we finished successfully
	}
	else
	{
		bNeedsLink = true;
		TheModule.bAttemptedBuild = true;
		TheModule.bBuiltSuccessfully = true; // Default to true. Will be set to false later on if failed.
	}

	std::cout << "Building module " << TheModule.Name << std::endl;

	// Spawn some threads to compile the source files
	for (unsigned int ThreadSpawn = 0; ThreadSpawn < CompileThreadCount; ThreadSpawn++)
	{
		std::thread* NewThread = new std::thread([this, OutputDirectory, &TheModule, &OutWriteLock, &SourceAcquisitionLock, SourcesNeedingBuild, CompletionIndex, CurrentSourceIndex]()
			{
				while (true)
				{
					std::string SourceNeedingBuild;

					bool bContinue = false;
					SourceAcquisitionLock.lock();
					{
						if (*CurrentSourceIndex < SourcesNeedingBuild->size())
						{
							SourceNeedingBuild = (*SourcesNeedingBuild)[*CurrentSourceIndex];
							(*CurrentSourceIndex)++;
							bContinue = true;
						}
					}
					SourceAcquisitionLock.unlock();

					if (!bContinue)
					{
						break;
					}

					Filesystem::path SourcePath(SourceNeedingBuild);

					std::string OutputBuff;
					std::string ErrorBuff;

					bool bResult = BuildSingleSource(TheModule.Name, OutputDirectory, SourceNeedingBuild, OutputBuff, ErrorBuff);

					if (!bResult || !TheModule.HasCorrespondingObject(OutputDirectory, ObjectFileExtension, SourceNeedingBuild))
					{
						TheModule.bBuiltSuccessfully = false;
					}

					// Detect if we need to print out the stdout/stderr buffer
					bool bPrintOut = OutputBuff.find("warn") != std::string::npos || OutputBuff.find("err") != std::string::npos;
					bool bPrintErr = ErrorBuff.find("warn") != std::string::npos || ErrorBuff.find("err") != std::string::npos;
					OutWriteLock.lock();
					{
						
						// Print out a message showing the file index being compiled and the file name
						std::cout << "\t[" << (*CompletionIndex + 1) << " of " << SourcesNeedingBuild->size() << "] " << SourcePath.filename().string();
						(*CompletionIndex)++;

						if (!bResult)
						{
							std::cout << " [fail]";
						}

						std::cout << std::endl;

						if (bPrintOut)
						{
							std::cout << OutputBuff << std::endl << std::endl;
						}

						if (bPrintErr)
						{
							std::cerr << ErrorBuff << std::endl;
						}
					}
					OutWriteLock.unlock();

				}
			});

		ThreadPool.push_back(NewThread);
	}

	for (std::thread* Thread : ThreadPool)
	{
		Thread->join();
		delete Thread;
	}

	delete CurrentSourceIndex;
	delete CompletionIndex;
	delete SourcesNeedingBuild;

	return TheModule.bBuiltSuccessfully;
}

bool AbstractBuildTool::BuildModule(std::string ModuleName)
{
	if (Modules.find(ModuleName) == Modules.end())
	{
		std::cerr << "Module " << ModuleName << " not added with AddModule(). Cannot build." << std::endl;
		return false;
	}
	
	Module& TheModule = *Modules[ModuleName];
	
	// Skip past modules that have already been visited
	if (TheModule.bVisisted)
	{
		return true;
	}
	else
	{
		// Mark this module as visited so we don't act on it again
		TheModule.bVisisted = true;
	}

	std::string ModuleBinaryDir = GetModuleBinaryDir(TheModule);
	std::string ModuleObjectDir = GetModuleObjectDir(TheModule);
	std::string ModuleLibraryDir = GetModuleLibraryDir(TheModule);

	// We need to rebuild this module regardless if one of the dependencies was built due to needing updated linkage
	bool bNeedsBuildDueToChild = false;
	bool bNeedsFullRebuild = false;

	// Check that module dependencies have been built and build if not
	for (const std::string& Dep : TheModule.ModuleDependencies)
	{
		if (Modules.find(Dep) == Modules.end())
		{
			std::cerr << "Module dependency " << Dep << " not found" << std::endl;

			return false;
		}
		else
		{
			Module* Mod = Modules[Dep];

			if (Mod)
			{
				// Only attempt to build the module if it hasn't already been visited

				if (Mod->bVisisted)
				{
					if (!Mod->bBuiltSuccessfully)
					{
						return false;
					}
					else if (Mod->bNeededFullRebuild)
					{
						bNeedsFullRebuild = true;
					}
				}
				else 
				{
					if (BuildModule(Mod->Name))
					{
						// Check if the module needed a full rebuild
						if(Mod->bNeededFullRebuild)
						{
							bNeedsFullRebuild = true;
						}
					}
					else
					{
						return false;
					}
				}

				// Mark this module for needing rebuild due the child module being rebuilt
				if (Mod->bAttemptedBuild)
				{
					bNeedsBuildDueToChild = true;
				}
			}

		}
	}

	// Create binaries directory
	Filesystem::create_directories(ModuleBinaryDir); // Where final binaries go
	Filesystem::create_directories(ModuleLibraryDir); // Where static libs go
	Filesystem::create_directories(ModuleObjectDir); // Where object code goes

	bool bNeedsLink; /* This indicates whether CompileModule() did any work */
	if(!CompileModule(TheModule, GetModuleObjectDir(TheModule), bNeedsLink, bNeedsFullRebuild))
	{
		return false;
	}

	TheModule.bNeededFullRebuild = bNeedsFullRebuild;
	
	if(bNeedsLink || bNeedsBuildDueToChild)
	{
		return LinkModule(TheModule);
	}
	else
	{
		return true;
	}

	
}

bool AbstractBuildTool::BuildAll(std::vector<std::string>& ModulesFailed)
{
	// Get all the modules
	std::vector<Module*> Modules;
	std::vector<Module*> EngineModules;

	if(!Filesystem::exists(RootPath))
	{
		std::cerr << "Modules path " << RootPath << " does not exist!" << std::endl;
		return false;
	}

	// Discover modules in root path of project.
	LoadModules(RootPath, Modules, &Settings);
	if (!VerifyModules(Modules)) // Make sure modules are correct (no duplicate names)
	{
		return false;
	}

	bool bIsBuildingEngine = (RootPath == Filesystem::path(GetEngineModulesDir()));

	// This only relavant if we're not building the engine. If we are, we have already loaded the engine modules above.
	if (!bIsBuildingEngine)
	{
		// Root modules different from root engine, load engine modules

		std::string EngineModulesPath;

		if(Settings.bDistribute)
		{
			// Load ONLY runtime modules here since we're building a distribution build
			EngineModulesPath = GetEngineRuntimeModulesDir();
		}
		else
		{
			EngineModulesPath = GetEngineModulesDir();
		}

		if (!bIsBuildingEngine && Filesystem::exists(EngineModulesPath))
		{
			LoadModules(EngineModulesPath, EngineModules, &Settings);
			if (!VerifyModules(EngineModules))
			{
				return false;
			}
		}
	}

	// Add our own modules
	for (Module* Module : Modules)
	{
		AddModule(Module);
	}

	// Add all engine modules (this will be empty if we're building the engine itself)
	for (Module* Module : EngineModules)
	{
		AddModule(Module);
	}

	std::string ErrorMsg;
	if(!CheckDependenies(ErrorMsg))
	{
		std::cerr << ErrorMsg << std::endl;
		return false;
	}

	// Check for circular dependencies
	std::vector<Module*> CircularChain;
	if (CheckCircular(CircularChain))
	{
		std::cerr << "Detected circular dependency between modules: ";

		for(int Index = 0; Index < CircularChain.size(); Index++)
		{
			Module* Mod = CircularChain[Index];
			if(Index < CircularChain.size() - 1)
			{
				std::cerr << Mod->Name << ", ";
			}
			else
			{
				std::cerr << Mod->Name << std::endl;
			}
		}
		
		return false;
	}

	// Defer return value so we can clean up modules right after.
	bool bSuccess = true;

	// Determine whether to build standalone or modular
	if (Settings.Type == BuildType::STANDALONE)
	{
		bSuccess = BuildAllStandalone();
	}
	else if (Settings.Type == BuildType::MODULAR)
	{
		bSuccess = BuildAllModular(ModulesFailed);
	}

	// Delete all modules since we're done
	for (Module* Module : Modules)
	{
		delete Module;
	}

	return bSuccess;

}

bool AbstractBuildTool::CheckCircularHelper(CircularHelper* Helper, std::vector<Module*>& Chain, std::unordered_set<std::string>& Visited, std::vector<CircularHelper*>& Allocated)
{
	Module* Mod = Helper->Mod;
	Visited.insert(Mod->Name);

	for(std::string Dep : Mod->ModuleDependencies)
	{
		Module* ModDep = Modules[Dep];

		if(Visited.find(ModDep->Name) != Visited.end())
		{
			// Unravel the chain
			std::vector<Module*> TempChain;
			
			bool bIsCircular = false;
			CircularHelper* Unravel = Helper;
			do
			{
				TempChain.push_back(Unravel->Mod);
				Unravel = Unravel->Parent;
			} while (Unravel && Unravel->Mod != ModDep);

			if(Unravel)
			{
				// Push the last mod onto the chain
				TempChain.push_back(Unravel->Mod);
				
				Chain = TempChain;
				return true;
			}
		}
		else
		{
			// Create a new circular helper
			CircularHelper* New = new CircularHelper;
			New->Mod = ModDep;
			New->Parent = Helper;

			Allocated.push_back(New);

			if(CheckCircularHelper(New, Chain, Visited, Allocated))
			{
				return true;
			}
		}
	}

	return false;
}

bool AbstractBuildTool::CheckCircular(std::vector<Module*>& Chain)
{
	// std::unordered_map<std::string, bool> ModulesVisited;
	for(auto& Mod : Modules)
	{
		std::unordered_set<std::string> Visited;
		
		Module* TheMod = Mod.second;
		std::vector<CircularHelper*> Allocated;

		CircularHelper* Initial = new CircularHelper;
		Initial->Mod = TheMod;
		Initial->Parent = nullptr;
		
		Allocated.push_back(Initial);

		bool bHadCircular = CheckCircularHelper(Initial, Chain, Visited, Allocated);

		// Clean up circular allocators
		for(CircularHelper* Circular : Allocated)
		{
			delete Circular;
		}

		if(bHadCircular)
		{
			return true;
		}
	}

	// Mark all modules as visited=false
	for(auto& Mod : Modules)
	{
		Mod.second->bVisisted = false;
	}

	return false;
}

bool AbstractBuildTool::CheckDependenies(std::string& ErrorMsg)
{
	for(auto& ModPair : Modules)
	{
		Module* Mod = ModPair.second;

		for(std::string& DepString : Mod->ModuleDependencies)
		{
			Module* ModDep = Modules[DepString];

			if(!ModDep)
			{
				ErrorMsg = "Module " + Mod->Name + " depends on non-existent module " + DepString;
				return false;
			}
		}
	}

	return true;
}

bool AbstractBuildTool::BuildAllStandalone()
{
	// Build the game in standalone mode, as a single exe
	std::string StandaloneBinaryDir = "";
	std::string StandaloneObjectDir = "";

	if (Settings.OutputDirectory.empty())
	{
		StandaloneBinaryDir = BinaryDir;
		StandaloneObjectDir = ObjectDirectory;
	}
	else
	{
		StandaloneBinaryDir = Settings.OutputDirectory;
		StandaloneObjectDir = (Filesystem::path(StandaloneBinaryDir) / "Object").string();
	}
	
	Filesystem::create_directories(StandaloneBinaryDir); // Where final binaries go
	Filesystem::create_directories(StandaloneObjectDir); // Where object code goes

	// Locate the individual source files within this module that need to be built
	// Could be all of them if a header changed

	bool bBuiltSuccessfully = true;

	// Create generated source for all modules
	if(!CreateGeneratedModuleSource())
	{
		std::cerr << "Failed to generate source" << std::endl;
		return false;
	}

	// Compile all modules. Order doesn't matter here since they're all being linked together inevitably.
	// Discover source for all of the modules
	for (auto& Mod : Modules)
	{

		// We can ignore both below as we're always going to build the executable
		bool bNeedsLink = false;
		bool bNeedsFullRebuild = false;
		
		if (!CompileModule(*Mod.second, StandaloneObjectDir, bNeedsLink, bNeedsFullRebuild))
		{
			bBuiltSuccessfully = false;
		}
	}

	if (!bBuiltSuccessfully)
	{
		return false;
	}

	bool bSuccess = LinkStandalone(StandaloneBinaryDir, StandaloneObjectDir, Settings.StandaloneName);

	// Delete object directory always for standalone builds.
	// In the future, I may want to handle a better way of separating standalone binaries from modular binaries
	// But for now, standalone is only used for distribution builds so it's not high priority
	Filesystem::remove_all(StandaloneObjectDir);

	// If we built successfully, copy all ThirdParty binaries into the output directory
	if (bSuccess)
	{
		CopyBinaries(StandaloneBinaryDir);
	}

	return bSuccess;
}

bool AbstractBuildTool::BuildAllModular(std::vector<std::string>& ModulesFailed)
{
	bool bSuccess = true;

	if(Modules.size() > 0)
	{

		// Top sort all modules and generate the reflection data in order
		std::vector<std::string> AllModules;
		std::vector<std::string> ModulesTopSorted;

		auto ModulesItr = Modules.begin();
		while(ModulesItr != Modules.end())
		{
			AllModules.push_back(ModulesItr->first);
			++ModulesItr;
		}

		TopSort(AllModules, Modules, ModulesTopSorted);

		if (!CreateGeneratedModuleSource())
		{
			std::cerr << "Failed to generate source" << std::endl;
			return false;
		}
		
		for (auto& Mod : Modules)
		{
			//bool bNeedsFullRebuild = false; // Ignore this, we're building top level modules

			// Build the module by name (first in the pair)
			if (!BuildModule(Mod.first))
			{
				bSuccess = false;
				ModulesFailed.push_back(Mod.first);
			}
		}
	}
	else
	{
		std::cerr << "No modules found in " << RootPath << std::endl;
	}

	// If we succeeded, copy all binaries into the binary directory
	if (bSuccess)
	{
		// Todo: Binaries should go wherever the executable is located, this may be a fair assumption for now
		CopyBinaries(GetEngineBinaryDir());
	}

	return bSuccess;
}

std::string AbstractBuildTool::GetModuleBinaryDir(const Module& Mod)
{
//	return BinaryDir;

	return Mod.bEngineModule ? GetEngineBinaryDir() : BinaryDir;
}

std::string AbstractBuildTool::GetModuleLibraryDir(const Module& Mod)
{
	return Mod.bEngineModule ? GetEngineLibraryDir() : LibraryDir;
}

std::string AbstractBuildTool::GetModuleObjectDir(const Module& Mod)
{
	return Mod.bEngineModule ? GetEngineObjectDir() : ObjectDirectory;
}

bool RunBuild(std::string RootDir, std::vector<std::string>& Options)
{
	// Determine which build tool to use
	AbstractBuildTool* BuildTool = nullptr;

	BuildSettings Settings;
	Settings.Config = BuildConfiguration::DEVELOPMENT;
	Settings.Type = BuildType::MODULAR;

	// Detect host platform
	Settings.HostPlatform.OS   = GetHostOS();
	Settings.HostPlatform.Arch = GetHostArchitecture();
	Settings.Toolset           = GetHostToolset();

	// Initialize the target platform to the host platform by default
	Settings.TargetPlatform = Settings.HostPlatform;

	// Check for target architecture changes
	if(HasOption(Options, "-TargetArch"))
	{
		std::string TargetArch = ParseUniqueOption(Options, "-TargetArch");

		if(TargetArch == "x86")
		{
			Settings.TargetPlatform.Arch = ArchitectureType::X86;
		}
		else if (TargetArch == "x86_64")
		{
			Settings.TargetPlatform.Arch = ArchitectureType::X64;
		}
		else if (TargetArch == "Arm")
		{
			Settings.TargetPlatform.Arch = ArchitectureType::ARM;
		}
		else if (TargetArch == "Arm64")
		{
			Settings.TargetPlatform.Arch = ArchitectureType::ARM64;
		}
		else
		{
			std::cerr << "Unrecognized target architecture " << TargetArch << std::endl;
			return false;
		}

	}

	// Check for target OS changes
	if (HasOption(Options, "-TargetOS"))
	{
		std::string TargetOS = ParseUniqueOption(Options, "-TargetOS");

		if (TargetOS == "Windows")
		{
			Settings.TargetPlatform.OS = OSType::WINDOWS;
		}
		else if (TargetOS == "Linux")
		{
			Settings.TargetPlatform.OS = OSType::LINUX;
		}
		else if (TargetOS == "OSX")
		{
			Settings.TargetPlatform.OS = OSType::OSX;
		}
		else
		{
			std::cerr << "Unrecognized target OS " << TargetOS << std::endl;
			return false;
		}

	}

	if(HasOption(Options, "-BuildConfig"))
	{
		std::string BuildConfig = ParseUniqueOption(Options, "-BuildConfig");

		if(BuildConfig == "Development")
		{
			Settings.Config = BuildConfiguration::DEVELOPMENT;
		}
		else if(BuildConfig == "Shipping")
		{
			Settings.Config = BuildConfiguration::SHIPPING;
		}
		else
		{
			std::cerr << "Unrecognized build configuration " << BuildConfig << std::endl;
			return false;
		}
	}

	if (HasOption(Options, "-BuildType"))
	{
		std::string BuildType = ParseUniqueOption(Options, "-BuildType");

		if (BuildType == "Modular")
		{
			Settings.Type = BuildType::MODULAR;
		}
		else if (BuildType == "Standalone")
		{
			Settings.Type = BuildType::STANDALONE;
		}
		else
		{
			std::cerr << "Unrecognized build type " << BuildType << std::endl;
			return false;
		}
	}

	if (HasOption(Options, "-OutputDirectory"))
	{
		Settings.OutputDirectory = ParseUniqueOption(Options, "-OutputDirectory");
	}
	else
	{
		// Leave empty so build tool can figure it out
		Settings.OutputDirectory = "";
	}

	// Determine the build toolset
	// TODO: compile build toolset into a -key=value parameter
	if (HasOption(Options, "-GCC"))
	{
		Settings.Toolset = ToolsetType::GCC;
	}

	if (HasOption(Options, "-MSVC"))
	{
		Settings.Toolset = ToolsetType::MSVC;
	}

	if (HasOption(Options, "-Clang"))
	{
		Settings.Toolset = ToolsetType::CLANG;
	}

	if (HasOption(Options, "-Distribute"))
	{
		Settings.bDistribute = true;
	}

	std::string Compat = Settings.CheckCompatability();

	if(!Compat.empty())
	{
		std::cerr << "Build compatibility error: " << Compat << std::endl;
		return false;
	}

	if(Settings.Toolset == ToolsetType::MSVC)
	{
#ifdef RBUILD_HOST_OS_WINDOWS
		BuildTool = new MSVCBuildTool(RootDir, Settings);
#endif
	}
	else if(Settings.Toolset == ToolsetType::GCC)
	{
		BuildTool = new GCCBuildTool(RootDir, Settings);
	}
	
	if(!BuildTool)
	{
		std::cerr << "No build tool specified" << std::endl;
		return false;
	}
	else
	{
		std::vector<std::string> ModulesFailed;
		bool bSuccess = BuildTool->BuildAll(ModulesFailed);
		
		if(bSuccess)
		{
			std::cerr << "Build succeeded" << std::endl;
		}
		else
		{
			if(ModulesFailed.size() > 0)
			{
				std::cerr << "Modules failed: ";
				for(int Index = 0; Index < ModulesFailed.size(); Index++)
				{
					if(Index < ModulesFailed.size() - 1)
					{
						std::cerr << ModulesFailed[Index] << ", ";
					}
					else
					{
						std::cerr << ModulesFailed[Index] << std::endl;
					}
				}
			}
			else
			{
				std::cerr << "Build failed" << std::endl;
			}
		}

		return bSuccess;
	}
}

bool BuildCmd(std::vector<std::string>& Args)
{
	if (Args.size() < 2)
	{
		IncorrectUsage("build <RootPath> [-options]");
		return false;
	}

	std::string RootPath = Args[1];
	std::vector<std::string> SubVec(Args.begin() + 1, Args.end());
	
	return RunBuild(RootPath, SubVec);
}

bool CleanAll(std::string RootDir)
{

	Filesystem::path ModulesRootParent = Filesystem::absolute(Filesystem::path(RootDir)).parent_path();
	std::string BinaryDir = Filesystem::absolute(ModulesRootParent / "Binary").string();
	std::string LibsDir = Filesystem::absolute(ModulesRootParent / "Intermediate" / "Libraries").string();
	std::string ObjectFilesDir = Filesystem::absolute(ModulesRootParent / "Intermediate" / "Object").string();
	std::string GeneratedDir = Filesystem::absolute(ModulesRootParent / "Intermediate" / "Generated").string();

	std::error_code FileError;

	Filesystem::remove_all(GeneratedDir, FileError);

	if (FileError)
	{
		std::cerr << "Failed to clean generated directory" << std::endl;
	}

	// Clean data put in module folders
	Filesystem::remove_all(BinaryDir, FileError);

	if (FileError)
	{
		std::cerr << "Failed to clean binary directory" << std::endl;
	}

	Filesystem::remove_all(ObjectFilesDir, FileError);

	if (FileError)
	{
		std::cerr << "Failed to clean intermediate directory" << std::endl;
	}

	Filesystem::remove_all(LibsDir, FileError);

	if (FileError)
	{
		std::cerr << "Failed to clean libraries directory" << std::endl;
	}

	return true;
}

bool CleanCmd(std::vector<std::string>& Args)
{
	if (Args.size() < 2)
	{
		IncorrectUsage("clean <RootPath>");
		return false;
	}

	// Run build all
	return CleanAll(Args[1]);
}

bool RebuildCmd(std::vector<std::string>& Args)
{
	if (Args.size() < 2)
	{
		IncorrectUsage("rebuild <RootPath> [-options]");
		return false;
	}

	std::string RootPath = Args[1];
	std::vector<std::string> SubVec(Args.begin() + 1, Args.end());

	// Run build all
	CleanAll(RootPath);
	return RunBuild(RootPath, SubVec);
}