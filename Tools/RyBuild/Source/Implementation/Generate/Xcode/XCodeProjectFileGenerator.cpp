#include "Generate/Xcode/XCodeProjectFileGenerator.h"
#include <iostream>
#include "Common.h"
#include <fstream>
#include <cstdint>
#include <random>
#include <sstream>
#include "Util/Util.h"
#include "Build/Module/Module.h"
#include <unordered_set>

constexpr char* ProjectName = "AryzeEngine";
static std::unordered_set<std::string> GeneratedUuids;

std::random_device Rand;
std::default_random_engine Engine(Rand());
std::uniform_int_distribution<uint64_t> Dist(0, UINT64_MAX);

struct XCodeData
{
	Filesystem::path RootAbs;
	Filesystem::path XCodeProjDir;
	Filesystem::path PbxFile;
};

class XCodeUUID
{

public:

	XCodeUUID() : Upper32(0), Lower64(0) {};

	static XCodeUUID Create()
	{
		auto Generate = []()
		{
			XCodeUUID Result;
			Result.Lower64 = Dist(Engine);
			Result.Upper32 = Dist(Engine) & 0xFFFFFFFF;
			return Result;
		};

		std::string LastGenerated = "";
		XCodeUUID Result;

		// Ensure there is no UUID collision (rarer than winning the lottery thousands of times, but still might as well handle the case)
		do
		{
			Result = Generate();
			LastGenerated = Result.ToString();
		} while (GeneratedUuids.find(LastGenerated) != GeneratedUuids.end());
		

		return Result;
	}

	std::string ToString()
	{
		std::ostringstream Out;
		Out << std::setfill('0') << std::hex << std::setw(8) << Upper32 << std::setw(16) << Lower64;

		// Convert to upper case
		return ToUpper(Out.str());
	}

private:

	uint32_t Upper32;
	uint64_t Lower64;
};

class PbxWriter
{

public:

	PbxWriter(): Indentation(0), ProjectGuid(XCodeUUID::Create()), EngineExeReference(XCodeUUID::Create()) {};

	void SetEngineModules(const std::vector<Module*>& Mods)
	{
		for(Module* Mod : Mods)
		{
			ModuleData NewData;
			NewData.Mod = Mod;

			Modules.insert(std::make_pair(Mod->Name, NewData));
		}
	}

	void WriteFileReferences()
	{
		static auto AddFileReference = [this](Filesystem::path Path, std::string LastKnownFileType = "", std::string ExplicitFileType = "", std::string SourceTree = "", int IncludeInIndex = -1)
		{
			std::string Filename = Path.filename().string();

			PbxFileReference NewFile;
			NewFile.Path = Filename;
			NewFile.LastFileType = LastKnownFileType;
			NewFile.ExplicitFileType = ExplicitFileType;
			NewFile.SourceTree = SourceTree;
			NewFile.IncludeInIndex = IncludeInIndex;

			FileReferencesByUuid.insert(std::make_pair(NewFile.Uuid.ToString(), NewFile));
			FileReferencesByPath.insert(std::make_pair(Filesystem::canonical(Path).string(), NewFile));

			return NewFile.Uuid;
		};

		for(auto& Module : Modules)
		{
			// Discover headers and source
			Module.second.PythonBuildScriptPath = Module.second.Mod->GetPythonBuildScriptPath();
			Module.second.Mod->DiscoverHeaders(Module.second.Headers);
			Module.second.Mod->DiscoverSource(Module.second.Sources);

			for(std::string HeaderFile : Module.second.Headers)
				AddFileReference(Filesystem::path (HeaderFile), "sourcecode.cpp.h", "", "\"<group>\"");

			for (std::string SourceFile : Module.second.Sources)
				AddFileReference(Filesystem::path(SourceFile), "sourcecode.cpp.cpp", "", "\"<group>\"");

			AddFileReference(Filesystem::path(Module.second.PythonBuildScriptPath), "text.script.python", "", "\"<group>\"");
		}

		// Add AryzeEngine product file reference
		//EngineExeReference = AddFileReference("AryzeEngine", "", "compiled.mach-o.executable", "BUILT_PRODUCTS_DIR", 0);

		// Generate file references
		WriteLineIgnoreIndents("/* Begin PBXFileReferences section */");

		for(auto& FileRef : FileReferencesByUuid)
		{
			std::string Line = FileRef.first + " /* " + FileRef.second.Path + " */ ";
			Line += "= {isa = PBXFileReference; ";

			if(!FileRef.second.LastFileType.empty())
				Line += "lastKnownFileType = " + FileRef.second.LastFileType + "; ";
			if (!FileRef.second.ExplicitFileType.empty())
				Line += "explicitFileType = " + FileRef.second.ExplicitFileType + "; ";
			if(FileRef.second.IncludeInIndex >= 0)
				Line += "includeInIndex = " + std::to_string(FileRef.second.IncludeInIndex) + "; ";

			Line += "path = " + FileRef.second.Path + "; ";
			Line += "sourceTree = " + FileRef.second.SourceTree + "; ";

			Line += "};";

			WriteLine(Line);
		}
		WriteLineIgnoreIndents("/* End PBXFileReferences section */");
	}

	bool AddPbxGroupUnique(Filesystem::path Path, XCodeUUID& OutUuid, bool bIsRoot = false)
	{
		// Only look at directories
		if (!Filesystem::is_directory(Path))
			return false;
		
		std::string Canon = Filesystem::canonical(Path).string();
		if (GroupsByUuid.find(Canon) == GroupsByUuid.end())
		{
			PbxGroup NewGroup;
			NewGroup.Foldername = Path.filename().string();
			NewGroup.SourceTree = "<group>";
			NewGroup.bIsRoot = bIsRoot;

			// Find child file references
			Filesystem::directory_iterator DirItr(Canon);
			for (auto& ChildPath : DirItr)
			{
				std::string ChildCanon = Filesystem::canonical(ChildPath).string();
				if(ChildPath.is_directory())
				{
					// If child doesn't exist, recursively create it
					if (GroupsByPath.find(ChildCanon) == GroupsByPath.end())
					{
						XCodeUUID Res;
						AddPbxGroupUnique(ChildCanon, Res);
					}

					// Insert child
					PbxGroup Child = GroupsByPath.at(ChildCanon);
					NewGroup.Children.push_back(Child.Uuid.ToString());
				}
				else if(FileReferencesByPath.find(ChildCanon) != FileReferencesByPath.end()) // Look for the file reference
				{
					PbxFileReference FileRef = FileReferencesByPath.at(ChildCanon);
					NewGroup.Children.push_back(FileRef.Uuid.ToString());
				}
			}

			GroupsByUuid.insert(std::make_pair(NewGroup.Uuid.ToString(), NewGroup));
			GroupsByPath.insert(std::make_pair(Canon, NewGroup));

			OutUuid = NewGroup.Uuid;
			return true;
		}
		return false;
	}

	std::string GetFilenameFromUuid(std::string Uuid)
	{
		if (FileReferencesByUuid.find(Uuid) != FileReferencesByUuid.end())
			return FileReferencesByUuid.at(Uuid).Path;
		if (GroupsByUuid.find(Uuid) != GroupsByUuid.end())
			return GroupsByUuid.at(Uuid).Foldername;
	}

	void WriteGroups()
	{
		// Add engine modules group
		if(!AddPbxGroupUnique(GetEngineModulesDir(), ModulesRootPbxGroup))
		{
			std::cerr << "Failed to add PbxGroup for engine modules directory" << std::endl;
			return;
		}

		// Write out PBX groups
		WriteLineIgnoreIndents("/* Begin PBXGroup section */");
		for(auto& PbxGroup : GroupsByUuid)
		{
			std::string PbxGroupStartLine = PbxGroup.first;
			if(!PbxGroup.second.bIsRoot)
			{
				PbxGroupStartLine += " /* " + GetFilenameFromUuid(PbxGroup.first) + " */ ";
			}

			WriteLineAddIndent(PbxGroupStartLine + "= {");
			{
				WriteLine("isa = PBXGroup;");

				WriteLineAddIndent("children = (");
				{
					for(int Child = 0; Child < PbxGroup.second.Children.size(); Child++)
					{
						std::string ChildUuid = PbxGroup.second.Children.at(Child);
						std::string Line = ChildUuid + " /* " + GetFilenameFromUuid(ChildUuid) + " */";
						if(Child < PbxGroup.second.Children.size() - 1)
						{
							Line += ",";
						}
						WriteLine(Line);
					}
				}
				WriteLineRemoveIndent(");");

				if (!PbxGroup.second.bIsRoot)
				{
					WriteLine("path = " + PbxGroup.second.Foldername + ";");
				}

				WriteLine("sourceTree = \"" + PbxGroup.second.SourceTree + "\";");
			}
			WriteLineRemoveIndent("};");
		}
		WriteLineIgnoreIndents("/* End PBXGroup section */");
	}

	void WriteProject()
	{
		XCodeUUID DebugTargetUuid = XCodeUUID::Create();
		XCodeUUID DebugProjectUuid = XCodeUUID::Create();

		WriteLineAddIndent(ProjectGuid.ToString() + " /* Project Object */ = {");
		{
			WriteLine("isa = PBXProject;");
			WriteLine("developmentRegion = en;");
			WriteLine("hasScannedForEncodings = 0;");
			WriteLine("buildConfigurationList = " + ProjectConfigListUuid.ToString() + ";");
			WriteLine("projectRoot = \"\";"); // Always same directory
			WriteLine("projectDirPath = \"\";"); // Always same directory
			WriteLine("mainGroup = " + ModulesRootPbxGroup.ToString() + ";"); // Main files root
			WriteLineAddIndent("targets = (");
			{
				WriteLine(NativeTargetUuid.ToString());
			}
			WriteLineRemoveIndent(");");
		}
		WriteLineRemoveIndent("};");

		//USER_HEADER_SEARCH_PATHS = test2;

		// Generate include paths
		std::string IncludePaths;
		for(auto& Mod : Modules)
		{
			IncludePaths += Mod.second.Mod->GetIncludeDir() + " ";
			IncludePaths += Mod.second.Mod->GetGeneratedDir() + " ";
		}

		// Create build configurations
		WriteLineAddIndent(DebugTargetUuid.ToString() + " = {");
		{
			WriteLine("isa = XCBuildConfiguration;");
			WriteLineAddIndent("buildSettings = {");
			{
				WriteLine("PRODUCT_NAME = \"$(TARGET_NAME)\";");
				WriteLine("HEADER_SEARCH_PATHS = \"" + IncludePaths + "\";");
			}
			WriteLineRemoveIndent("};");
			WriteLine("name = Debug;");
		}
		WriteLineRemoveIndent("};");

		WriteLineAddIndent(TargetConfigListUuid.ToString() + " = {");
		{
			WriteLine("isa = XCConfigurationList;");
			WriteLineAddIndent("buildConfigurations = (");
			{
				WriteLine(DebugTargetUuid.ToString());
			}
			WriteLineRemoveIndent(");");
			WriteLine("defaultConfigurationIsVisible = 0;");
			WriteLine("defaultConfigurationName = Debug;");
		}
		WriteLineRemoveIndent("};");

		// Create build configurations
		WriteLineAddIndent(DebugProjectUuid.ToString() + " = {");
		{
			WriteLine("isa = XCBuildConfiguration;");
			WriteLineAddIndent("buildSettings = {");
			{
				WriteLine("PRODUCT_NAME = \"$(TARGET_NAME)\";");
			}
			WriteLineRemoveIndent("};");
			WriteLine("name = Debug;");
		}
		WriteLineRemoveIndent("};");

		WriteLineAddIndent(ProjectConfigListUuid.ToString() + " = {");
		{
			WriteLine("isa = XCConfigurationList;");
			WriteLineAddIndent("buildConfigurations = (");
			{
				WriteLine(DebugProjectUuid.ToString());
			}
			WriteLineRemoveIndent(");");
			WriteLine("defaultConfigurationIsVisible = 0;");
			WriteLine("defaultConfigurationName = Debug;");
		}
		WriteLineRemoveIndent("};");

	}

	void WriteTargets()
	{
		NativeTargetUuid = XCodeUUID::Create();

		// Get path to build tool
		std::string BuildToolPath = GetRyBuildPath();
		std::string RyBuildArgs = "build ./Modules";
		std::string Script = BuildToolPath + " " + RyBuildArgs;

		XCodeUUID ScriptBuildPhase = XCodeUUID::Create();
		WriteLineAddIndent(ScriptBuildPhase.ToString() + " = {");
		{
			WriteLine("isa = PBXShellScriptBuildPhase;");
			WriteLine("buildActionMask = " + std::to_string(INT32_MAX) + ";");
			WriteLine("files = (");
			WriteLine(");");
			WriteLine("inputPaths = (");
			WriteLine(");");
			WriteLine("outputPaths = (");
			WriteLine(");");
			WriteLine("runOnlyForDeploymentPostprocessing = 0;");
			WriteLine("shellPath = /bin/sh;");
			WriteLine("shellScript = \"" + Script + "\";");
		}
		WriteLineRemoveIndent("};");

		// Write script build phase

		WriteLineAddIndent(NativeTargetUuid.ToString() + " = {");
		{
			WriteLine("isa = PBXNativeTarget;");
			WriteLine("buildConfigurationList = " + TargetConfigListUuid.ToString() + ";");
			WriteLine("buildRules = (");
			WriteLine(");");
			WriteLine("dependencies = (");
			WriteLine(");");
			WriteLineAddIndent("buildPhases = (");
			{
				WriteLine(ScriptBuildPhase.ToString());
			}
			WriteLineRemoveIndent(");");
			WriteLine("name = AryzeEngine;");
			WriteLine("productInstallPath = \"" + GetEngineBinaryDir() + "\";");
			WriteLine("productName = RyRuntime-EditorMain;");
			WriteLine("productReference = " + EngineExeReference.ToString() + ";");
			WriteLine("productType = \"com.apple.product-type.application\";"); // Executable, static lib, dynamic lib, etc.
		}
		WriteLineRemoveIndent("};");

	}

	void Write(std::string Output)
	{
		OutPbx.open(Output);

		TargetConfigListUuid = XCodeUUID::Create();
		ProjectConfigListUuid = XCodeUUID::Create();
		
		// UTF header
		WriteLine("// !$*UTF8*$!");
		WriteLineAddIndent("{");
		{
			// Archive		
			WriteLine("archiveVersion = 1;");

			WriteLine("classes = {");
			WriteLine("};");

			WriteLine("objectVersion = 55;");

			WriteLineAddIndent("objects = {");
			{
				// Write out file references
				WriteLineIgnoreIndents("");
				WriteFileReferences();
				WriteLineIgnoreIndents("");

				// Write out groups
				WriteLineIgnoreIndents("");
				WriteGroups();
				WriteLineIgnoreIndents("");

				// Write out native targets
				WriteTargets();

				// Write out project
				WriteProject();
				WriteLineIgnoreIndents("");

				// Write out build phase

				// Write out XC build configuration

				// Write out configuration list

				// Write out root object (project)
				PbxGroup Group = GroupsByPath.at(Filesystem::canonical(GetEngineModulesDir()).string());
			}
			WriteLineRemoveIndent("};");

			WriteLine("rootObject = " + ProjectGuid.ToString() + "; ");
		}				
		// Write out root object
		WriteLineRemoveIndent("}");

		OutPbx.close();
	}

private:

	struct PbxFileReference
	{
		XCodeUUID Uuid;
		std::string LastFileType;
		std::string ExplicitFileType;
		std::string Path; // This is really just the name
		std::string SourceTree;
		int IncludeInIndex;

		PbxFileReference():
		Uuid(XCodeUUID::Create())
		{
		}

		PbxFileReference(const PbxFileReference& Ref):
		Uuid(Ref.Uuid),
		LastFileType(Ref.LastFileType),
		ExplicitFileType(Ref.ExplicitFileType),
		Path(Ref.Path),
		SourceTree(Ref.SourceTree)
		{
		}


	};

	struct PbxGroup
	{
		XCodeUUID Uuid;
		std::string Foldername;
		std::vector<std::string> Children; // Uuids of file references
		std::string SourceTree;
		bool bIsRoot = false;

		PbxGroup() :
		Uuid(XCodeUUID::Create())
		{
		}

		PbxGroup(const PbxGroup& Ref) :
		Uuid(Ref.Uuid),
		Foldername(Ref.Foldername),
		Children(Ref.Children),
		SourceTree(Ref.SourceTree)
		{
		}
	};	

	struct ModuleData
	{
		Module* Mod;
		std::vector<std::string> Headers;
		std::vector<std::string> Sources;
		std::string PythonBuildScriptPath;
	};

	XCodeUUID TargetConfigListUuid; // Target level configuration list object
	XCodeUUID ProjectConfigListUuid; // Project level configuration list object
	XCodeUUID NativeTargetUuid; // Pbx Native target for engine
	XCodeUUID ProjectGuid; // Pbx project reference
	XCodeUUID EngineExeReference; // Product file reference
	XCodeUUID ModulesRootPbxGroup; // Root for project files

	std::unordered_map<std::string, PbxGroup> GroupsByPath;
	std::unordered_map<std::string, PbxGroup> GroupsByUuid;

	std::unordered_map<std::string, PbxFileReference> FileReferencesByUuid;
	std::unordered_map<std::string, PbxFileReference> FileReferencesByPath;

	std::unordered_map<std::string, ModuleData> Modules;
	

	void AddIndent()
	{
		Indentation++;
	}

	void RemoveIndent()
	{
		Indentation--;
	}

	void WriteLine(std::string Line)
	{
		for (int Indent = 0; Indent < Indentation; Indent++)
			OutPbx << "\t";

		OutPbx << Line << std::endl;
	}

	void WriteLineIgnoreIndents(std::string Line)
	{
		OutPbx << Line << std::endl;
	}

	void WriteLineAddIndent(std::string Line)
	{
		WriteLine(Line);
		AddIndent();
	}

	void WriteLineRemoveIndent(std::string Line)
	{
		RemoveIndent();
		WriteLine(Line);
	}

	std::ofstream OutPbx;
	int Indentation;
	
};

void CreatePbxProj(XCodeData& Data)
{
	Data.PbxFile = Data.XCodeProjDir / "project.pbxproj";

	std::string EngineModules = GetEngineModulesDir();
	std::vector<Module*> Modules;
	DiscoverModules(EngineModules, Modules);

	PbxWriter Writer;
	Writer.SetEngineModules(Modules);

	XCodeUUID Test = XCodeUUID::Create();
	std::cout << "UUID: " << Test.ToString() << std::endl;

	Writer.Write(Data.PbxFile.string());
}

bool GenerateEngineProjectFiles_XCode(std::string EngineRootPath, std::string Compiler)
{
	XCodeData Data;
	Data.RootAbs = Filesystem::canonical(EngineRootPath);
	Data.XCodeProjDir = Data.RootAbs / (ProjectName + std::string(".xcodeproj"));	

	// Create .xcodeproj directory
	Filesystem::create_directories(Data.XCodeProjDir);

	// Create pbxproj
	CreatePbxProj(Data);

	return false;
}

bool GenerateGameProjectFiles_XCode(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler)
{
	return false;
}