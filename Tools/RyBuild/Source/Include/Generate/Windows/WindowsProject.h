#pragma once

#include <string>
#include <fstream>
#include "WindowsProjectBase.h"
#include "Build/Module/Module.h"

namespace RyBuild
{
	
	class WindowsProject : public WindowsProjectBase
	{
	public:

		WindowsProject(const std::string& Name, const std::string& Directory);
		virtual ~WindowsProject() = default;

		bool GenerateProject() override;

		// WINDOWS VCPROJX PROJECT GENERATOR FUNCTIONS
		bool GenerateMainProject();
		bool GenerateFilters();
		bool GenerateUser();
		void WriteExtensionTargets();
		void WriteTargets();
		void EndItemGroup();
		void WriteCompileItems(std::string Directory);
		void WriteIncludeItems(std::string Directory);
		void WriteModuleFiles();
		void BeginItemGroup();
		void WriteNMakeProperties(std::string Config, std::string Platform);
		//void WriteConfigurationDefinition(std::string Config, std::string Platform);
		//void WriteConfigPropertyGroup(std::string Config, std::string Platform);
		void WriteConfigPropertyGroupLabel(std::string Config, std::string Platform);
		void WriteUserProps(std::string Config, std::string Platform);
		void WriteUserMacros();
		void WriteShared();
		void WriteExtensionSettings();
		void WriteProps();
		void WriteDefaultProps();
		void WriteProjectGlobals();
		void EndProjectConfigurations();
		void WriteConfiguration(std::string Configuration, std::string Debug);
		void BeginProjectConfigurations();
		void EndProject();
		void BeginProject();
		void PrintIndents();

		void SetModules(const std::vector<Module*>& Modules);

		/*
		 * Additional modules staged for intellisense.
		 */
		void SetIncludeModules(const std::vector<Module*>& Modules);

		void SetBuildCmdLine(const std::string& CmdLine);
		void SetRebuildCmdLine(const std::string& CmdLine);
		void SetCleanCmdLine(const std::string& CmdLine);
		void SetExecutable(const std::string& CmdLine);
		void SetWorkingDirectory(const std::string& CmdLine);
		void SetArguments(const std::string& CmdLine);

		/**
		 * Sets the directory that gets used as the actual module root.
		 *
		 * This can be any arbitrary location on the filesystem, so we needed extra support for this.
		 */
		void SetModuleRoot(std::string& ModuleRoot);
		
	private:

		std::vector<Module*> ProjectModules;
		std::vector<Module*> IncludeModules;

		std::string ModuleRoot;

		std::string BuildCmdLine;
		std::string RebuildCmdLine;
		std::string CleanCmdLine;
		std::string Executable;
		std::string WorkingDirectory;
		std::string Arguments;

		// The string GUID representation for this project used in the SLN and VCXPROJ
		std::string GUID;

		// Used in xml parsing -- should remove soon
		int IndentLevel;

		std::ofstream Output;

	};
}