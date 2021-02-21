#include "Common.h"
#include "Util/Util.h"

#ifdef RYBUILD_WINDOWS
#include "Generate/Windows/WindowsProject.h"

namespace RyBuild
{
	
	WindowsProject::WindowsProject(const std::string& Name, const std::string& Directory):
		WindowsProjectBase(Name, Directory),
		IndentLevel(0)
	{
		// We will assume each visual studio project each has its own set of modules, and thus, its own folder
		SetFolder(true);
		
		this->ModuleRoot = Filesystem::canonical(GetAbsoluteProjectPath()).string();
	}

	void WindowsProject::PrintIndents()
	{
		for (int Indent = 0; Indent < IndentLevel; Indent++)
		{
			Output << "\t";
		}
	}

	void WindowsProject::BeginProject()
	{
		Output << R"(<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">)" << std::endl;
		IndentLevel++;
	}

	void WindowsProject::EndProject()
	{
		Output << "</Project>" << std::endl;

		Output.close();
	}

	void WindowsProject::BeginProjectConfigurations()
	{
		PrintIndents();

		Output << "<ItemGroup Label = \"ProjectConfigurations\">" << std::endl;

		IndentLevel++;
	}

	void WindowsProject::WriteConfiguration(std::string Configuration, std::string Debug)
	{
		PrintIndents();
		Output << R"(<ProjectConfiguration Include="Debug|x64">)" << std::endl;

		IndentLevel++;
		PrintIndents();
		Output << R"(<Configuration>Debug</Configuration>)" << std::endl;
		PrintIndents();
		Output << R"(<Platform>x64</Platform>)" << std::endl;

		IndentLevel--;
		PrintIndents();
		Output << R"(</ProjectConfiguration>)" << std::endl;
	}

	void WindowsProject::EndProjectConfigurations()
	{
		IndentLevel--;
		PrintIndents();

		Output << "</ItemGroup>" << std::endl;
	}

	void WindowsProject::WriteProjectGlobals()
	{
		// Write out project globals
		PrintIndents();
		Output << R"(<PropertyGroup Label="Globals">)" << std::endl;

		IndentLevel++;
		PrintIndents();
		Output << R"(<VCProjectVersion>16.0</VCProjectVersion>)" << std::endl;
		PrintIndents();
		Output << R"(<ProjectGuid>{)" << GUID << R"(}</ProjectGuid>)" << std::endl;
		//PrintIndents();
		//Output << R"(<RootNamespace>)" << GetProjectName() << R"(</RootNamespace>)" << std::endl;
		//PrintIndents();
		//Output << R"(<WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>)" << std::endl;

		IndentLevel--;
		PrintIndents();
		Output << R"(</PropertyGroup>)" << std::endl;
	}

	void WindowsProject::WriteDefaultProps()
	{
		PrintIndents();
		Output << R"(<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />)" << std::endl;
	}

	void WindowsProject::WriteProps()
	{
		PrintIndents();
		Output << R"(<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />)" << std::endl;
	}

	void WindowsProject::WriteExtensionSettings()
	{
		PrintIndents();
		Output << R"(<ImportGroup Label="ExtensionSettings">)" << std::endl;
		PrintIndents();
		Output << R"(</ImportGroup>)" << std::endl;
	}

	void WindowsProject::WriteShared()
	{
		PrintIndents();
		Output << R"(<ImportGroup Label="Shared">)" << std::endl;
		PrintIndents();
		Output << R"(</ImportGroup>)" << std::endl;
	}

	void WindowsProject::WriteUserMacros()
	{
		PrintIndents();
		Output << R"(<PropertyGroup Label="UserMacros" />)" << std::endl;
	}

	void WindowsProject::WriteUserProps(std::string Config, std::string Platform)
	{
		PrintIndents();
		Output << R"##(<ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'==')##" << Config << "|" << Platform << R"##('">)##" << std::endl;

		IndentLevel++;
		PrintIndents();
		Output << R"##(<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label = "LocalAppDataPlatform"/> )##" << std::endl;

		--IndentLevel;
		PrintIndents();
		Output << R"##(</ImportGroup>)##" << std::endl;
	}

	void WindowsProject::WriteConfigPropertyGroupLabel(std::string Config, std::string Platform)
	{
		PrintIndents();
		Output << R"(<PropertyGroup Condition="'$(Configuration)|$(Platform)'==')" << Config << "|" << Platform << R"('" Label="Configuration">)" << std::endl;
		IndentLevel++;

		PrintIndents();
		Output << R"(<ConfigurationType>Makefile</ConfigurationType>)" << std::endl;
		PrintIndents();
		Output << R"(<UseDebugLibraries>true</UseDebugLibraries>)" << std::endl;
		PrintIndents();
		Output << R"(<PlatformToolset>v142</PlatformToolset>)" << std::endl;
		//PrintIndents();
		//Output << R"(<CharacterSet>MultiByte</CharacterSet>)" << std::endl;

		IndentLevel--;
		PrintIndents();
		Output << "</PropertyGroup>";
	}

	void WindowsProject::WriteNMakeProperties(std::string Config, std::string Platform)
	{
		PrintIndents();
		Output << R"(<PropertyGroup Condition="'$(Configuration)|$(Platform)'==')" << Config << "|" << Platform << R"('">)" << std::endl;
		IndentLevel++;
		{
			PrintIndents();

			Output << R"(<NMakeOutput>)" << Executable << R"(</NMakeOutput>)" << std::endl;
			// Output << R"(<NMakeOutput>)" << "$(SolutionDir)Binary\\RyRuntime-TestGame.exe" << R"(</NMakeOutput>)" << std::endl;
			PrintIndents();

			Output << R"(<NMakePreprocessorDefinitions>_Debug;_WINDLL;$(NMakePreprocessorDefinitions)</NMakePreprocessorDefinitions>)" << std::endl;
			PrintIndents();

			Output << R"(<OutDir>$(SolutionDir)Binary\</OutDir>)" << std::endl;
			PrintIndents();

			Output << R"(<IntDir>$(SolutionDir)Intermediate\ProjectFiles\</IntDir>)" << std::endl;
			PrintIndents();

			Output << "<NMakeBuildCommandLine>" << BuildCmdLine << "</NMakeBuildCommandLine>" << std::endl;
			PrintIndents();

			Output << "<NMakeRebuildCommandLine>" << RebuildCmdLine << "</NMakeRebuildCommandLine>" << std::endl;
			PrintIndents();
			
			Output << "<NMakeCleanCommandLine>" << CleanCmdLine << "</NMakeCleanCommandLine>" << std::endl;
			PrintIndents();

			// Build the search include directories string for intellisense
			{
				Output << "<NMakeIncludeSearchPath>";
				std::string IncludeDirectories;

				// Stage both combined and project modules for intellisense 
				std::vector<Module*> CombinedModules;
				for (Module* Mod : IncludeModules)
					CombinedModules.push_back(Mod);
				for (Module* Mod : ProjectModules)
					CombinedModules.push_back(Mod);

				for (const Module* Mod : CombinedModules)
				{
					// Add the modules include and third party directories (if any)
					Output << Mod->GetIncludeDir() << ";";

					// Add the module's generated directory
					Output << Mod->GetGeneratedDir() << ";";

					// Add module engine external include paths
					for (const ExternDependency& Extern : Mod->ExternDependencies)
					{
						Output << Extern.GetIncludePath() << ";";
					}

					
					if(!Mod->PythonIncludes.empty())
					{
						for(const std::string& ThirdPartyIncludeDir : Mod->PythonIncludes)
						{
							std::string ThirdPartyDirAbs = Filesystem::absolute(Filesystem::path(Mod->GetThirdPartyDir()) / ThirdPartyIncludeDir).string();
							Output << ThirdPartyDirAbs << ";";
						}
					}
				}
				Output << "$(NMakeIncludeSearchPath)</NMakeIncludeSearchPath>" << std::endl;
			}
			
		}

		--IndentLevel;
		PrintIndents();
		Output << R"(</PropertyGroup>)" << std::endl;
	}

	void WindowsProject::BeginItemGroup()
	{
		PrintIndents();
		Output << "<ItemGroup>" << std::endl;
		IndentLevel++;
	}

	void WindowsProject::WriteIncludeItems(std::string Directory)
	{
		std::vector<std::string> ProjectFiles;

		// Accumulate headers in all project modules
		for(Module* Mod : ProjectModules)
		{
			Mod->DiscoverHeaders(ProjectFiles, true);
		}
		
		for (std::string GlobFile : ProjectFiles)
		{
			PrintIndents();
			Output << "<ClInclude Include=\"" << GlobFile << "\" />" << std::endl;
		}
	}

	void WindowsProject::WriteCompileItems(std::string Directory)
	{
		std::vector<std::string> ProjectFiles;

		// Accumulate headers in all project modules
		for (Module* Mod : ProjectModules)
		{
			Mod->DiscoverSource(ProjectFiles, true);
		}

		for (std::string GlobFile : ProjectFiles)
		{
			PrintIndents();
			Output << "<ClCompile Include=\"" << GlobFile << "\" />" << std::endl;
		}
	}

	void WindowsProject::WriteModuleFiles()
	{
		std::vector<std::string> ModuleFiles;

		// Accumulate headers in all project modules
		for (Module* Mod : ProjectModules)
		{
			ModuleFiles.push_back(Mod->ModuleFilePath);
		}

		for (std::string File : ModuleFiles)
		{
			PrintIndents();
			Output << "<None Include=\"" << File << "\" />" << std::endl;
		}
	}

	void WindowsProject::EndItemGroup()
	{
		IndentLevel--;
		PrintIndents();
		Output << "</ItemGroup>" << std::endl;
	}

	void WindowsProject::WriteTargets()
	{
		PrintIndents();
		Output << R"(<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />)" << std::endl;
	}

	void WindowsProject::WriteExtensionTargets()
	{
		PrintIndents();
		Output << R"(<ImportGroup Label="ExtensionTargets">)" << std::endl;
		Output << R"(</ImportGroup>)" << std::endl;
	}

	bool WindowsProject::GenerateMainProject()
	{
		std::string ProjectFilePathAbs = (Filesystem::canonical(GetArtifactDirectory()) / (GetPhysicalName() + ".vcxproj")).string();
		std::string ProjectFileParentPath = Filesystem::path(ProjectFilePathAbs).parent_path().string();
		
		// std::string ProjectFilePath = ProjectFileDirectory + "\\" + ProjectFileName + ".vcxproj";
		// std::string ParentPath = std::experimental::filesystem::path(ProjectFilePath).parent_path().string();
		
		std::error_code Error;
		if (!std::experimental::filesystem::create_directories(ProjectFileParentPath, Error))
		{
			if (Error != std::error_code())
			{
				return false;
			}
		}
		
		// Open the output stream
		Output.open(ProjectFilePathAbs);

		// Write XML header
		Output << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

		BeginProject();
		{

			// Write out project configurations
			BeginProjectConfigurations();
			{
				WriteConfiguration("Debug", "x64");
			}
			EndProjectConfigurations();

			WriteProjectGlobals();

			WriteDefaultProps();

			// Write out configurations
			WriteConfigPropertyGroupLabel("Debug", "x64");

			WriteProps();

			WriteExtensionSettings();
			WriteShared();

			// User properties
			WriteUserProps("Debug", "x64");

			WriteUserMacros();

			WriteNMakeProperties("Debug", "x64");

			// Write configuration details
			//WriteConfigPropertyGroup("Debug", "x64");

			// More configuration details
			//WriteConfigurationDefinition("Debug", "x64");

			// Write files for source
			{
				// Write out include files
				BeginItemGroup();
				{
					WriteIncludeItems(".");
				}
				EndItemGroup();

				// Write out compile files
				BeginItemGroup();
				{
					WriteCompileItems(".");
					//WriteCompileItems(".");
				}
				EndItemGroup();

				// Write out module files
				BeginItemGroup();
				{
					WriteModuleFiles();
					//WriteCompileItems(".");
				}
				EndItemGroup();
			}

			WriteTargets();
			WriteExtensionTargets();
		}
		EndProject();

		return true;
	}

	bool WindowsProject::GenerateUser()
	{
		std::string UserPath = GetArtifactDirectory() + "\\" + GetPhysicalName() + ".vcxproj.user";

		Output.open(UserPath);
		{
			Output << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
			Output << R"(<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">)" << std::endl;
			{
				Output << "\t<PropertyGroup>" << std::endl;
				{
					Output << "\t\t<LocalDebuggerWorkingDirectory>";
					{
						Output << WorkingDirectory;
					}
					Output << "</LocalDebuggerWorkingDirectory>" << std::endl;

					Output << "\t\t<LocalDebuggerCommandArguments>";
					{
						Output << Arguments;
					}
					Output << "</LocalDebuggerCommandArguments>" << std::endl;

				}
				Output << "\t</PropertyGroup>" << std::endl;
			}
			Output << R"(</Project>)" << std::endl;
			
		}
		Output.close();

		return true;
	}

	bool WindowsProject::GenerateFilters()
	{
		std::string FiltersPath = GetArtifactDirectory() + "\\" + GetPhysicalName() + ".vcxproj.filters";

		Output.open(FiltersPath);

		// Create a mapping between source file and filter
		// The filter is the path prior to the source file name, but after the module root
		std::map<std::string, std::vector<std::string>> FiltersToSource;

		// Use the actual name of the project as the root of the modules
		std::string CanonPrefix = Filesystem::canonical(ModuleRoot).string();

		for(Module* Mod : ProjectModules)
		{
			std::vector<std::string> FilterableFiles;

			Mod->DiscoverSource(FilterableFiles, true);
			Mod->DiscoverHeaders(FilterableFiles, true);
			FilterableFiles.push_back(Mod->ModuleFilePath);

			// Add the module third party to the files to include

			for(std::string& Source : FilterableFiles)
			{
				std::string SourceFile = Filesystem::absolute(Filesystem::path(Source)).string();
				std::string CanonParent = Filesystem::canonical(Source).parent_path().string();
				std::string Filter = CanonParent.substr(CanonPrefix.size() + 1); // Skip over the starting \\

				// Add or create a new vector for the new source file
				if(FiltersToSource.find(Filter) != FiltersToSource.end())
				{
					FiltersToSource[Filter].push_back(SourceFile);
				}
				else
				{
					FiltersToSource[Filter] = std::vector<std::string> {SourceFile};
				}
				
			}

		}

		// Generate all sub filters
		for (auto MapItr = FiltersToSource.begin(); MapItr != FiltersToSource.end(); ++MapItr)
		{
			std::string Filter = MapItr->first;

			while(Filesystem::path(Filter).has_parent_path())
			{
				std::string NewFilter = Filesystem::path(Filter).parent_path().string();

				if(FiltersToSource.find(NewFilter) == FiltersToSource.end())
				{
					// Insert empty vector
					FiltersToSource[NewFilter] = std::vector<std::string>();
				}

				Filter = NewFilter;
			}
		}
		
		// Write XML header
		Output << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		Output << R"(<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">)" << std::endl;
		{
			// Iterate through filters
			Output << "\t<ItemGroup>" << std::endl;
			{
				for (auto MapItr = FiltersToSource.begin(); MapItr != FiltersToSource.end(); ++MapItr)
				{
					const std::string& Filter = MapItr->first;
					Output << std::string("\t\t<Filter Include=\"") + Filter + "\">" << std::endl;
					{
						// Generate UUID
						std::string FilterGuid = CreateGuid();
						Output << "\t\t\t<UniqueIdentifier>{" + FilterGuid + "}</UniqueIdentifier>" << std::endl;
					}
					Output << "\t\t</Filter>" << std::endl;
				}
			}
			Output << "\t</ItemGroup>" << std::endl;

			// ClInclude group
			Output << "\t<ItemGroup>" << std::endl;
			{
				for (auto MapItr = FiltersToSource.begin(); MapItr != FiltersToSource.end(); ++MapItr)
				{
					const std::string& Filter = MapItr->first;

					std::string Middle = "\t\t\t<Filter>" + Filter + "</Filter>";

					for (const std::string SourceFile : MapItr->second)
					{
						if(Filesystem::path(SourceFile).extension() == ".h")
						{
							Output << std::string("\t\t<ClInclude Include=\"") + SourceFile + "\">" << std::endl;
							{
								Output << Middle << std::endl;
							}
							Output << "\t\t</ClInclude>" << std::endl;
						}
						else if (Filesystem::path(SourceFile).extension() == ".cpp" || Filesystem::path(SourceFile).extension() == ".hpp")
						{
							Output << std::string("\t\t<ClCompile Include=\"") + SourceFile + "\">" << std::endl;
							{
								Output << Middle << std::endl;
							}
							Output << "\t\t</ClCompile>" << std::endl;
						}
						else if(Filesystem::path(SourceFile).extension() == ".module")
						{
							Output << std::string("\t\t<None Include=\"") + SourceFile + "\">" << std::endl;
							{
								Output << Middle << std::endl;
							}
							Output << "\t\t</None>" << std::endl;
						}
					}
				}
			}
			Output << "\t</ItemGroup>" << std::endl;

			// ClCompile group
			// Output << "\t<ItemGroup>" << std::endl;
			// {
			// 	for (auto MapItr = FiltersToSource.begin(); MapItr != FiltersToSource.end(); ++MapItr)
			// 	{
			// 		const std::string& Filter = MapItr->first;
			//
			// 		for (const std::string SourceFile : MapItr->second)
			// 		{
			// 			if (Filesystem::path(SourceFile).extension() == ".cpp" || Filesystem::path(SourceFile).extension() == ".hpp")
			// 			{
			// 				Output << std::string("\t\t<ClCompile Include=\"") + SourceFile + "\">" << std::endl;
			// 				{
			// 					Output << "\t\t\t<Filter>" + Filter + "</Filter>" << std::endl;
			// 				}
			// 				Output << "\t\t</ClCompile>" << std::endl;
			// 			}
			// 		}
			// 	}
			// }
			//
			// // Text group
			// Output << "\t<ItemGroup>" << std::endl;
			// {
			// 	for (auto MapItr = FiltersToSource.begin(); MapItr != FiltersToSource.end(); ++MapItr)
			// 	{
			// 		const std::string& Filter = MapItr->first;
			//
			// 		for (const std::string SourceFile : MapItr->second)
			// 		{
			// 			if (Filesystem::path(SourceFile).extension() == ".cpp" || Filesystem::path(SourceFile).extension() == ".hpp")
			// 			{
			// 				Output << std::string("\t\t<ClCompile Include=\"") + SourceFile + "\">" << std::endl;
			// 				{
			// 					Output << "\t\t\t<Filter>" + Filter + "</Filter>" << std::endl;
			// 				}
			// 				Output << "\t\t</ClCompile>" << std::endl;
			// 			}
			// 		}
			// 	}
			// }
			// Output << "\t</ItemGroup>" << std::endl;
		}
		Output << R"(</Project>)" << std::endl;

		Output.close();

		return true;
	}

	bool WindowsProject::GenerateProject()
	{
		if(!GenerateMainProject())
		{
			return false;
		}

		if(!GenerateFilters())
		{
			return false;
		}

		if (!GenerateUser())
		{
			return false;
		}

		return true;
	}

	void WindowsProject::SetModules(const std::vector<Module*>& Modules)
	{
		this->ProjectModules = Modules;
	}

	void WindowsProject::SetIncludeModules(const std::vector<Module*>& Modules)
	{
		this->IncludeModules = Modules;
	}

	void WindowsProject::SetBuildCmdLine(const std::string& CmdLine)
	{
		this->BuildCmdLine = CmdLine;
	}
	
	void WindowsProject::SetRebuildCmdLine(const std::string& CmdLine)
	{
		this->RebuildCmdLine = CmdLine;
	}

	void WindowsProject::SetCleanCmdLine(const std::string& CmdLine)
	{
		this->CleanCmdLine = CmdLine;
	}

	void WindowsProject::SetExecutable(const std::string& CmdLine)
	{
		this->Executable = CmdLine;
	}

	void WindowsProject::SetWorkingDirectory(const std::string& CmdLine)
	{
		this->WorkingDirectory = CmdLine;
	}

	void WindowsProject::SetArguments(const std::string& CmdLine)
	{
		this->Arguments = CmdLine;
	}

	void WindowsProject::SetModuleRoot(std::string& ModuleRoot)
	{
		this->ModuleRoot = ModuleRoot;
	}
	
}

#endif