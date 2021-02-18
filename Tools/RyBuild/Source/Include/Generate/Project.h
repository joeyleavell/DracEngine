#pragma once

#include <string>
#include <vector>
#include <functional>

namespace RyBuild
{

	/**
	 * Defines the structure and substructure for an abstract project hierarchy.
	 */
	class Project
	{

	public:

		Project(const std::string& Name, const std::string& Directory);
		virtual ~Project() = default;

		/**
		 * The name of this project. 
		 */
		const std::string& GetProjectName() const;

		/**
		 * This is used in cases where the physical artifact name differs from the 'name' of the project.
		 *
		 * If this is not set explicitly, it will be the same as project name.
		 */
		const std::string& GetPhysicalName() const;

		/**
		 * Where the root of the entire project structure is stored.
		 */
		const std::string& GetRootDirectory() const;

		/**
		 * Where files generated for this project should be placed. If this has not been set, the artifact
		 * directory will be the same as the physical filepath created by the project hierarchy.
		 *
		 * For example, a project with name B inheriting from project name A's project files will be placed in the directory ./A/B
		 */
		const std::string GetArtifactDirectory() const;

		/**
		 * @return The parent project
		 */
		const Project* GetParent() const;

		/**
		 * Whether this project represents a folder in the project hierarchy.
		 */
		bool IsFolder() const;

		std::string GetRelativeProjectPath() const;
		std::string GetAbsoluteProjectPath() const;

		void SetProjectName(const std::string& ProjectName);

		void SetPhysicalName(const std::string& PhysicalName);

		void SetArtifactDirectory(const std::string& ArtDir);

		void SetFolder(bool bIsFolder);

		void ForEachProject(std::function<void(Project*)> ForEach, bool bRecursive);

		void AddSubProject(Project* SubProject);
		Project* FindSubProjectByName(const std::string& ProjectName);

		/**
		 * Generates just this project.
		 *
		 * @return Whether or not the generator succeeded.
		 */
		virtual bool GenerateProject() = 0;

		/**
		 * Generates this project and all sub-projects.
		 *
		 * @return Whether or not the generator succeeded.
		 */
		virtual bool GenerateProjects();

	private:

		bool bIsFolder;

		std::string ProjectName;
		std::string PhysicalName;
		std::string RootDirectory;
		std::string ArtifactDirectory;

		std::vector<Project*> SubProjects;
		std::vector<std::string> AdditionalIncludes;

		Project* Parent;
	};
}

