#include "Common.h"

#ifdef RBUILD_HOST_OS_WINDOWS

#include "Generate/Windows/WindowsProjectSolution.h"
#include "Generate/Windows/WindowsProject.h"
#include "Generate/Windows/WindowsProjectFolder.h"
#include <fstream>

namespace RyBuild
{
	
	bool WindowsProjectSolution::GenerateProject()
	{
		std::string SlnPath = (Filesystem::path(GetAbsoluteProjectPath()) / (GetProjectName() + ".sln")).string();
		// The solution name is the same as the project name
		std::ofstream Output(SlnPath);

		// Print header 
		Output << "\nMicrosoft Visual Studio Solution File, Format Version 12.00" << std::endl;
		Output << "# Visual Studio Version 16" << std::endl;
		Output << "VisualStudioVersion = 16.0.29418.71" << std::endl;
		Output << "MinimumVisualStudioVersion = 10.0.40219.1" << std::endl;

		// Generate projects
		ForEachProject([&Output, SlnPath](Project* Next)
		{
			auto AsWindowsBase = dynamic_cast<WindowsProjectBase*>(Next);
			auto AsWindowsVcxproj = dynamic_cast<WindowsProject*>(Next);
			auto AsWindowsFolder = dynamic_cast<WindowsProjectFolder*>(Next);

			if (!AsWindowsVcxproj && !AsWindowsFolder)
				return;

			std::string GuidString = AsWindowsBase->GetGUID();
			std::string NameString = AsWindowsBase->GetPhysicalName();
			std::string PathString = (Filesystem::path(AsWindowsVcxproj->GetArtifactDirectory()) / (AsWindowsVcxproj->GetPhysicalName() + ".vcxproj")).string();
			std::string TypeGuid = "";

			if(AsWindowsVcxproj)
			{
				TypeGuid = CPP_UUID;
			}
			else if(AsWindowsFolder)
			{
				TypeGuid = PROJECT_FOLDER_UUID;
			}

			Output << "Project(\"{" + TypeGuid + "}\") = \"" << NameString << "\", \"" << PathString << "\", \"{" + GuidString + "}\"" << std::endl;
			Output << "EndProject" << std::endl;
			
		}, true);
		
		Output << "Global" << std::endl;
		{
			// Global configuration platforms
			Output << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << std::endl;
			{
				Output << "\t\tDebug|x64 = Debug|x64" << std::endl;
			}
			Output << "\tEndGlobalSection" << std::endl;

			Output << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution" << std::endl;
			{
				
				// Generate global project sections
				ForEachProject([&Output](Project* Next)
				{
					auto AsWindowsVcxproj = dynamic_cast<WindowsProject*>(Next);

					if (!AsWindowsVcxproj)
						return;

					std::string GuidString = AsWindowsVcxproj->GetGUID();

					Output << "\t\t{" << GuidString << "}.Debug|x64.ActiveCfg = Debug|x64" << std::endl;
					Output << "\t\t{" << GuidString << "}.Debug|x64.Build.0 = Debug|x64" << std::endl;
					
				}, true);
				
			}
			Output << "\tEndGlobalSection" << std::endl;

			Output << R"(	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection)" << std::endl;

			// Nested projects
			Output << "\tGlobalSection(NestedProjects) = preSolution" << std::endl;
			{
				// Generate global project sections
				ForEachProject([&Output](Project* Next)
				{
					if (!Next)
						return;

					auto AsWindowsBase = dynamic_cast<WindowsProjectBase*>(Next);
					auto AsWindowsBaseParent = dynamic_cast<const WindowsProjectBase*>(Next->GetParent());

					if(AsWindowsBase && AsWindowsBaseParent && AsWindowsBaseParent->GetParent())
					{
						std::string ChildGuid = "{" + AsWindowsBase->GetGUID() + "}";
						std::string ParentGuid = "{" + AsWindowsBaseParent->GetGUID() + "}";

						Output << "\t\t" << ChildGuid << " = " << ParentGuid << std::endl;
					}
				}, true);
			}
			Output << "\tEndGlobalSection" << std::endl;

			// Solution notes section
			Output << R"(	GlobalSection(ExtensibilityGlobals) = postSolution
		SolutionGuid = {)" << GetGUID() << R"(}
	EndGlobalSection)" << std::endl;

		}
		Output << "EndGlobal" << std::endl;

		Output.close();

		return true;
	}

}

#endif