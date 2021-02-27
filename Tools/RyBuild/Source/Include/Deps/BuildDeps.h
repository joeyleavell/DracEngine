#pragma once
#include <string>
#include <vector>

/**
 * At the moment, only best guess is implemented.
 */
enum class ArtifactLocatorMethod
{
	BestGuess, // Uses the name of the dependency as the best guess.
	NameHint, // Treats the individual name fields as "hints"
	FullName, // Treats the name fields as full names, but can be in any directory
	RelativePath, // Treats the name fields as strict paths to a location/directory
};

enum class Architecture
{
	All,
	x86_64,
	x86,
	Arm
};

enum class BuildOS
{
	All,
	Windows,
	Linux
};

enum class Toolset
{
	All,
	MSVC,
	GCC
};

struct Platform
{
	Architecture Arch;
	Toolset Tool;
	BuildOS OS;
};

enum class GitLabelType
{
	None,
	Tag,
	Branch
};

struct Dependency
{
	std::string Name;
	
	/**
	 * The path of the git project to be cloned.
	 */
	std::string GitPath;

	/**
	 * The tag to checkout when cloning the Git project.
	 */
	std::string GitLabel;

	/**
	 * This determines what the value in GitLabel represents.
	 */
	GitLabelType LabelType = GitLabelType::Tag;
	
	/**
	 * The root directory of the cmake project. This should be stored relative to the root of the project.
	 */
	std::string CMakeRoot = ".";

	/**
	 * The path that will be considered the "includes" directory that should be copied into 
	 * 
	 */
	std::vector<std::string> IncludesDirectories;

	/**
	 * Commands to run prior to building dependency but after cloning.
	 */
	std::vector<std::vector<std::string>> PreBuildCommands;

	bool bRunCMakeInstall = false;
	
	bool bRunCMakeBuild = true;

	// Whether to install the produced so/dll's. Could be useful to turn off if it's installed by the system.
	bool bInstallSharedLibs = true;

	bool bInstallIncludes = true;

	/**
	 * The method to use when locating build artifacts.
	 */
	ArtifactLocatorMethod LocatorMethod = ArtifactLocatorMethod::BestGuess;

	/**
	 * Platforms to exclude building this dependency for.
	 */
	std::vector<Platform> ExcludedPlatforms;

	std::vector<std::string> CMakeGenArgs;

	/**
	 * If locator method is best guess, this field is ignored.
	 *
	 * An expected name of the produced lib. Does not need to include .dll or lib prefix as those will
	 * all be checked for.
	 */
	std::vector<std::string> LibNames;

	/**
	 * If locator method is best guess, this field is ignored.
	 *
	 * An expected name of the produced DLL. Does not need to include .dll as that will
	 * be checked for.
	 */
	std::vector<std::string> SharedLibNames;

	/**
	 * An expected name of the produced SO. Does not need to include .so or lib prefix as those will
	 * all be checked for.
	 */
	//std::vector<std::string> SONames;

};

bool BuildDepsCmd(std::vector<std::string>& Args);