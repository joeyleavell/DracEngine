#include "Deps/BuildDeps.h"
#include "Util/Util.h"
#include "Common.h"
#include <iostream>
#include <thread>

Platform GetTargetPlatform(Toolset Tools)
{
	Platform Target;
#if defined(RYBUILD_Arch_x86_64)
	Target.Arch = Architecture::x86_64;
#elif defined(RYBUILD_Arch_x86)
	Target.Arch = Architecture::x86;
#elif defined(RYBUILD_Arch_Arm)
	Target.Arch = Architecture::Arm;
#endif

#if defined(RYBUILD_WINDOWS)
	Target.OS = BuildOS::Windows;
#elif defined(RYBUILD_LINUX)
	Target.OS = BuildOS::Linux;
#elif defined(RYBUILD_MAC)
	//PlatformPath /= "Mac";
#endif

	Target.Tool = Tools;

	return Target;
}

bool IsPlatformExcluded(const std::vector<Platform>& Exclusions, const Platform& P)
{
	for (const Platform& Exclusion : Exclusions)
	{
		if (Exclusion.Arch == Architecture::All || Exclusion.Arch == P.Arch)
		{
			if (Exclusion.OS == BuildOS::All || Exclusion.OS == P.OS)
			{
				if (Exclusion.Tool == Toolset::All || Exclusion.Tool == P.Tool)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool GitCloneDep(Dependency& Dep, std::string& RootPath)
{
	std::string GitDir = (Filesystem::path(RootPath) / ".git").string();

	// Check if there is already a .git here
	// If there is, skip cloning
	if(!Filesystem::exists(GitDir))
	{
		std::vector<std::string> CloneArgs;
		CloneArgs.push_back("clone");
		CloneArgs.push_back(Dep.GitPath);
		CloneArgs.push_back(RootPath);

		if (!ExecProc("git", CloneArgs))
		{
			std::cerr << "Git clone failed" << std::endl;
			return false;
		}

		// Only checkout if working tree is clean
		// otherwise, could cause problems with building targets concurrently
		// This clone is considered temporary anyways and should not be used to develop
		if (Dep.LabelType != GitLabelType::None)
		{
			std::vector<std::string> CheckoutArgs;
			CheckoutArgs.push_back("--work-tree=" + RootPath);
			CheckoutArgs.push_back("--git-dir=" + GitDir);

			CheckoutArgs.push_back("checkout");

			if (Dep.LabelType == GitLabelType::Tag)
			{
				CheckoutArgs.push_back("tags/" + Dep.GitLabel);
			}
			else if (Dep.LabelType == GitLabelType::Branch)
			{
				CheckoutArgs.push_back(Dep.GitLabel);
			}

			if (!ExecProc("git", CheckoutArgs))
			{
				std::cerr << "Git checkout failed" << std::endl;
				return false;
			}
		}
	}

	return true;
}

Filesystem::path GetPlatformPath(Toolset Tools)
{
	Filesystem::path PlatformPath = "";

	// Detect host platform (cross compiling isn't supported for now)
#if defined(RYBUILD_Arch_x86_64)
	PlatformPath /= "x64";
#elif defined(RYBUILD_Arch_x86)
	PlatformPath /= "x86";
#elif defined(RYBUILD_Arch_Arm)
	PlatformPath /= "Arm";
#endif

#if defined(RYBUILD_WINDOWS)
	PlatformPath /= "Windows";
#elif defined(RYBUILD_LINUX)
	PlatformPath /= "Linux";
#elif defined(RYBUILD_MAC)
	PlatformPath /= "Mac";
#endif

	if(Tools == Toolset::GCC)
	{
		PlatformPath /= "GCC";
	}
	else if(Tools == Toolset::MSVC)
	{
		PlatformPath /= "MSVC";

	}

	return PlatformPath;
}

bool CMakeGenerate(Dependency& Dep, std::string RootDirectory, Toolset Tools)
{
	std::vector<std::string> CMakeGenerateArgs;

	// Add position independent code flag if on linux since we'll be using these in SOs
#ifdef RYBUILD_LINUX
	CMakeGenerateArgs.push_back("-D");
	CMakeGenerateArgs.push_back("CMAKE_POSITION_INDEPENDENT_CODE=ON");
#endif

	if(Tools == Toolset::MSVC)
	{
#ifdef RYBUILD_LINUX
		std::cerr << "MSVC not supported on Linux" << std::endl;
		return false;
#endif
	}
	else if(Tools == Toolset::GCC)
	{
#if defined(RYBUILD_WINDOWS)
		CMakeGenerateArgs.push_back("-G");
		CMakeGenerateArgs.push_back("\"MinGW Makefiles\"");
		std::cout << "Using MinGW to compile dependencies" << std::endl;
#endif
	}
	else
	{
		std::cerr << "Toolset not recognized: " << (int)Tools << std::endl;
		return false;
	}

	Filesystem::path SourceDir = Filesystem::path(RootDirectory) / Dep.CMakeRoot;
	Filesystem::path BuildDir = Filesystem::path(RootDirectory) / "Build" / GetPlatformPath(Tools);

	if (!Filesystem::exists(BuildDir))
	{
		Filesystem::create_directories(BuildDir);
	}

	std::cout << ("-H\"" + Filesystem::canonical(SourceDir).string() + "\"") << std::endl;

	Filesystem::path SourceRelativeToWD = PathRelativeTo(Filesystem::canonical("."), Filesystem::canonical(SourceDir));
	Filesystem::path BuildRelativeToWD = PathRelativeTo(Filesystem::canonical("."), Filesystem::canonical(BuildDir));

	CMakeGenerateArgs.push_back("-H" + SourceRelativeToWD.string());
	CMakeGenerateArgs.push_back("-B" + BuildRelativeToWD.string());

	// Add dependency options
	for(const std::string& CMakeOpt : Dep.CMakeGenArgs)
	{
		//CMakeGenerateArgs.push_back();
		CMakeGenerateArgs.push_back("-D" + CMakeOpt);
	}
		
	// Run cmake generate
	if (!ExecProc("cmake", CMakeGenerateArgs))
	{
		std::cerr << "CMake generate failed" << std::endl;
		return false;
	}

	return true;
}

bool CMakeBuild(Dependency& Dep, std::string RootDirectory, Toolset Tools)
{
	Filesystem::path BuildDir = Filesystem::path(RootDirectory) / "Build" / GetPlatformPath(Tools);
	Filesystem::path RelativeToWD = PathRelativeTo(Filesystem::canonical("."), BuildDir);
	
	std::vector<std::string> CMakeBuildArgs;
	CMakeBuildArgs.push_back("--build");
	CMakeBuildArgs.push_back(RelativeToWD.string());
	CMakeBuildArgs.push_back("--config");
	CMakeBuildArgs.push_back("Release");

	// Add native build arguments
	if(Tools == Toolset::GCC)
	{
		CMakeBuildArgs.push_back("--");
		
		unsigned int CoreCount = std::thread::hardware_concurrency();
		unsigned int ThreadCount = static_cast<unsigned int>(1.5 * CoreCount);
		CMakeBuildArgs.push_back(std::string("-j") + std::to_string(ThreadCount));
		std::cout << "Compiling with " << ThreadCount << " threads" << std::endl;
	}

	// Run cmake build
	if (!ExecProc("cmake", CMakeBuildArgs))
	{
		std::cerr << "CMake build failed" << std::endl;
		return false;
	}

	return true;
}

bool CMakeInstall(Dependency& Dep, std::string RootDirectory, Toolset Tools)
{
	Filesystem::path BuildDir = Filesystem::path(RootDirectory) / "Build" / GetPlatformPath(Tools);
	Filesystem::path RelativeToWD = PathRelativeTo(Filesystem::canonical("."), BuildDir);

	std::vector<std::string> CMakeBuildArgs;
	CMakeBuildArgs.push_back("--build");
	CMakeBuildArgs.push_back(RelativeToWD.string());
	CMakeBuildArgs.push_back("--target");
	CMakeBuildArgs.push_back("install");
	CMakeBuildArgs.push_back("--config");
	CMakeBuildArgs.push_back("Release");

	// Run cmake build
	if (!ExecProc("cmake", CMakeBuildArgs))
	{
		std::cerr << "CMake build failed" << std::endl;
		return false;
	}

	return true;
}

bool SharedLibTest_Ext(const std::string& Path)
{
	Filesystem::path FilePath = Filesystem::path(Path);

	if(FilePath.extension() == ".dll" || FilePath.extension() == ".so")
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool NameHintTest(const std::string& Path, std::string Hint)
{
	Filesystem::path FilePath = Filesystem::path(Path);

	std::string PathUpper = ToUpper(FilePath.stem().string());
	std::string HintUpper = ToUpper(Hint);

	if(PathUpper.find(HintUpper) != std::string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SharedLibTest(Dependency& Dep, const std::string& Path)
{
	if(!SharedLibTest_Ext(Path))
	{
		return false;
	}
	
	if(Dep.LocatorMethod == ArtifactLocatorMethod::BestGuess)
	{
		return NameHintTest(Path, Dep.Name);
	}
	else
	{
		bool bPassedTest = false;
		
		for (const std::string& LibName : Dep.SharedLibNames)
		{
			if (Dep.LocatorMethod == ArtifactLocatorMethod::NameHint)
			{
				if(NameHintTest(Path, LibName))
				{
					bPassedTest = true;
				}
			}
		}

		return bPassedTest;
	}

	return false;
}

bool StaticLibTest_Ext(const std::string& Path)
{
	Filesystem::path FilePath = Filesystem::path(Path);

	if (FilePath.extension() == ".lib" || FilePath.extension() == ".a")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool StaticLibTest(Dependency& Dep, const std::string& Path)
{
	if (!StaticLibTest_Ext(Path))
	{
		return false;
	}

	if (Dep.LocatorMethod == ArtifactLocatorMethod::BestGuess)
	{
		return NameHintTest(Path, Dep.Name);
	}
	else
	{
		bool bPassedTest = false;

		for (const std::string& LibName : Dep.LibNames)
		{
			if (Dep.LocatorMethod == ArtifactLocatorMethod::NameHint)
			{
				if(NameHintTest(Path, LibName))
				{
					bPassedTest = true;
				}
			}
			else if (Dep.LocatorMethod == ArtifactLocatorMethod::FullName)
			{
				// Todo: implement
			}
			else if (Dep.LocatorMethod == ArtifactLocatorMethod::RelativePath)
			{
				// Todo: implement
			}

		}

		return bPassedTest;
	}

	return false;	
}

void FindTargetSharedLibs(Dependency& Dep, std::string BuildDirectory, std::vector<std::string>& Out)
{
	if(!Dep.bInstallSharedLibs)
	{
		return;
	}
	
	Filesystem::recursive_directory_iterator BuildDirItr(BuildDirectory);

	for(auto& Path : BuildDirItr)
	{
		bool bResult = SharedLibTest(Dep, Path.path().string());
		if(bResult)
		{
			Out.push_back(Filesystem::absolute(Path).string());
		}
	}
	
}

void FindTargetStaticLibs(Dependency& Dep, std::string BuildDirectory, std::vector<std::string>& Out)
{
	Filesystem::recursive_directory_iterator BuildDirItr(BuildDirectory);

	for (auto& Path : BuildDirItr)
	{
		if (StaticLibTest(Dep, Path.path().string()))
		{
			Out.push_back(Filesystem::absolute(Path).string());
		}
	}
}

bool BuildDep(Dependency& Dep, std::string RootDir, Toolset Tools)
{
	// Don't build if this platform is excluded
	Platform TargetPlat = GetTargetPlatform(Tools);
	if (IsPlatformExcluded(Dep.ExcludedPlatforms, TargetPlat))
	{
		// Don't consider this a fail
		std::cout << "Dependency " << Dep.Name << " will not be built for platform " << GetPlatformPath(Tools).string() << std::endl;
		return true;
	}
	
	Filesystem::path BuildDir = Filesystem::path(RootDir) / "Build" / GetPlatformPath(Tools);

	// Git clone
	if(!GitCloneDep(Dep, RootDir))
	{
		return false;
	}
	
	// Cmake generate
	if(Dep.bRunCMakeBuild)
	{
		if (!CMakeGenerate(Dep, RootDir, Tools))
		{
			return false;
		}

		// Cmake build
		if (!CMakeBuild(Dep, RootDir, Tools))
		{
			return false;
		}

		if (Dep.bRunCMakeInstall)
		{
			if (!CMakeInstall(Dep, RootDir, Tools))
			{
				return false;
			}
		}

		// Find target artifacts (depends on platform)
		std::vector<std::string> SharedLibs;
		std::vector<std::string> StaticLibs;
		FindTargetSharedLibs(Dep, BuildDir.string(), SharedLibs);
		FindTargetStaticLibs(Dep, BuildDir.string(), StaticLibs);

		// Determine correct installation location
		std::string StaticLibsInstallLoc = (Filesystem::path(RootDir).parent_path().parent_path() / Dep.Name / "Libraries" / GetPlatformPath(Tools)).string();
		std::string SharedLibsInstallLoc = (Filesystem::path(RootDir).parent_path().parent_path() / Dep.Name / "Binaries" / GetPlatformPath(Tools)).string();

		// Install targets to location
		if (SharedLibs.size() > 0)
		{
			if (!Filesystem::exists(SharedLibsInstallLoc))
			{
				Filesystem::create_directories(SharedLibsInstallLoc);
			}
		}

		for (const std::string& SharedLib : SharedLibs)
		{
			Filesystem::path SharedLibPath = SharedLib;
			Filesystem::path InstallLoc = Filesystem::path(SharedLibsInstallLoc) / SharedLibPath.filename();

			if (Filesystem::exists(InstallLoc))
			{
				std::cerr << "Overwriting shared lib at " << InstallLoc << std::endl;
				Filesystem::remove(InstallLoc);
			}

			Filesystem::copy_file(SharedLib, InstallLoc);
		}

		if (StaticLibs.size() > 0)
		{
			if (!Filesystem::exists(StaticLibsInstallLoc))
			{
				Filesystem::create_directories(StaticLibsInstallLoc);
			}
		}

		for (const std::string& StaticLib : StaticLibs)
		{
			Filesystem::path StaticLibPath = StaticLib;
			Filesystem::path InstallLoc = Filesystem::path(StaticLibsInstallLoc) / StaticLibPath.filename();

			if(Filesystem::exists(InstallLoc))
			{
				std::cerr << "Overwriting static lib at " << InstallLoc << std::endl;
				Filesystem::remove(InstallLoc);
			}
			
			Filesystem::copy_file(StaticLib, InstallLoc);
		}
	}
	
	// Install include directories
	if(Dep.bInstallIncludes)
	{
		std::string IncludesInstallLoc = (Filesystem::path(RootDir).parent_path().parent_path() / Dep.Name / "Include").string();
		for (const std::string IncludePathOrDir : Dep.IncludesDirectories)
		{
			Filesystem::path Abs = Filesystem::canonical(Filesystem::path(RootDir) / IncludePathOrDir).string();

			if (Filesystem::is_directory(Abs))
			{
				// Copy all files in directory over
				Filesystem::recursive_directory_iterator DirItr(Abs);
				for (auto& SubPath : DirItr)
				{
					if (SubPath.path().extension() == ".hpp"
						|| SubPath.path().extension() == ".h"
						|| SubPath.path().extension() == ".c"
						|| SubPath.path().extension() == ".inl")
					{
						// Make file relative to abs
						Filesystem::path RelativeToPath = PathRelativeTo(Abs, Filesystem::canonical(SubPath));
						Filesystem::path InstallLoc = Filesystem::absolute(IncludesInstallLoc / RelativeToPath);

						if (!Filesystem::exists(InstallLoc))
						{
							std::cerr << "Installing include: " << InstallLoc.string() << std::endl;

							// Create install loc directories
							if (InstallLoc.has_parent_path())
							{
								create_directories(InstallLoc.parent_path());
							}

							Filesystem::copy_file(SubPath, InstallLoc);
						}

					}
				}
			}
		}
	}

	// Delete git clone

	return true;
}

bool BuildDeps(std::vector<Dependency>& Deps, std::string Dir, Toolset Tools)
{	
	Filesystem::path AbsDir = Filesystem::absolute(Dir);
	
	// Create the root directory
	if(!Filesystem::exists(AbsDir))
	{
		Filesystem::create_directories(AbsDir);
	}
	
	for(Dependency& Dep : Deps)
	{
		// Todo: check if dependency can be built for target platform

		std::string DepDir = (Filesystem::path(AbsDir) / "Repos" / Dep.Name).string();

		// If directory exists, error out
		// if (Filesystem::exists(DepDir))
		// {
		// 	std::cerr << "Directory " << DepDir << " already existed in root path" << std::endl;
		// 	return false;
		// }
		
		if(!BuildDep(Dep, DepDir, Tools))
		{
			return false;
		}
	}

	return true;
}

bool BuildDepsCmd(std::vector<std::string>& Args)
{	
	// Cut out first arg
	std::vector<std::string> DepsArgs;
	for(int Index = 1; Index < Args.size(); Index++)
	{
		DepsArgs.push_back(Args[Index]);
	}
	
	if(DepsArgs.size() < 1)
	{
		std::cerr << "Incorrect usage! Usage: deps [Options] <Path>" << std::endl;
		return false;
	}

	std::string BuildDir = FindNonOption(DepsArgs);
	Toolset Tools;

	if(HasOption(DepsArgs, "Toolset"))
	{
		std::string Parsed = ParseOption(DepsArgs, "Toolset");

		if(Parsed == "MSVC")
		{
			Tools = Toolset::MSVC;
		}
		else if(Parsed == "GCC")
		{
			Tools = Toolset::GCC;
		}
		else
		{
			std::cerr << "Toolset " << Parsed << " not recognized" << std::endl;
			return false;
		}
	}
	else
	{
#ifdef RYBUILD_WINDOWS
		Tools = Toolset::MSVC;
#elif defined(RYBUILD_LINUX)
		Tools = Toolset::GCC;
#endif
	}

	// Future todo: Store these dependencies in a file?
	// Create dependencies
	Dependency Bullet3;
	Bullet3.Name = "Bullet3";
	Bullet3.CMakeGenArgs = {
		"USE_MSVC_RUNTIME_LIBRARY_DLL=ON",
		"BUILD_BULLET2_DEMOS=OFF",
		"BUILD_CLSOCKET=OFF",
		"BUILD_CPU_DEMOS=OFF",
		"BUILD_ENET=OFF",
		"BUILD_EXTRAS=OFF",
		"BUILD_OPENGL3_DEMOS=OFF",
		"BUILD_PYBULLET=OFF",
		"BUILD_UNIT_TESTS=OFF"
	};
	Bullet3.LibNames = {"Bullet", "LinearMath"};
	Bullet3.LocatorMethod = ArtifactLocatorMethod::NameHint;
	Bullet3.IncludesDirectories = {"src/"};
	Bullet3.GitPath = "https://github.com/bulletphysics/bullet3.git";
	Bullet3.GitLabel = "3.08";

	Dependency Glew;
	Glew.Name = "Glew";
	Glew.CMakeGenArgs = {
		"glew-cmake_BUILD_STATIC=ON",
		"glew-cmake_BUILD_SHARED=OFF",
		"ONLY_LIBS=ON",
	};
	Glew.IncludesDirectories = {"src/"};
	Glew.GitPath = "https://github.com/Perlmint/glew-cmake.git";
	Glew.GitLabel = "glew-cmake-2.2.0";

	Dependency Glfw;
	Glfw.Name = "Glfw";
	Glfw.IncludesDirectories = {"include/GLFW"};
	Glfw.CMakeGenArgs = {
		"GLFW_BUILD_EXAMPLES=OFF",
		"GLFW_BUILD_TESTS=OFF",
		"GLFW_BUILD_DOCS=OFF",
		"BUILD_SHARED_LIBS=ON",
	};
	Glfw.GitPath = "https://github.com/glfw/glfw.git";
	Glfw.GitLabel = "3.3.3";

	Dependency Json;
	Json.Name = "Json";
	Json.bRunCMakeBuild = false;
	Json.GitPath = "https://github.com/nlohmann/json.git";
	Json.GitLabel = "v3.9.1";

	Dependency ShaderConductor;
	ShaderConductor.Name = "ShaderConductor";
	ShaderConductor.IncludesDirectories = {"Include/ShaderConductor"};
	ShaderConductor.LabelType = GitLabelType::None; // Get the latest of master, doesn't compile otherwise
	ShaderConductor.GitPath = "https://github.com/microsoft/ShaderConductor.git";
	ShaderConductor.GitLabel = "v0.2.0"; // Doesn't matter for now
	ShaderConductor.ExcludedPlatforms.push_back(Platform{ Architecture::All, Toolset::GCC, BuildOS::Windows });

	Dependency Glm;
	Glm.Name = "Glm";
	Glm.bRunCMakeBuild = false;
	Glm.IncludesDirectories = {"glm/"};
	Glm.GitPath = "https://github.com/g-truc/glm.git";
	Glm.GitLabel = "0.9.9.0";

	Filesystem::path VulkanHeadersPath = Filesystem::path(BuildDir) / "Repos/VulkanHeaders/Build/" / GetPlatformPath(Tools) / "install";
//	Filesystem::path RelativeToWD = PathRelativeTo(Filesystem::canonical("."), VulkanHeadersPath);

	if(!Filesystem::exists(VulkanHeadersPath))
	{
		Filesystem::create_directories(VulkanHeadersPath);
	}
	
	Dependency VulkanHeaders;
	VulkanHeaders.Name = "VulkanHeaders";
	VulkanHeaders.IncludesDirectories = {"include"};
	VulkanHeaders.CMakeGenArgs = {
		"CMAKE_INSTALL_PREFIX=" + Filesystem::canonical(VulkanHeadersPath).string()
	};
	VulkanHeaders.GitPath = "https://github.com/KhronosGroup/Vulkan-Headers.git";
	VulkanHeaders.GitLabel = "v1.2.170";
	VulkanHeaders.bInstallIncludes = true;
	VulkanHeaders.bRunCMakeInstall = true;
	
	Dependency VulkanLoader;
	VulkanLoader.Name = "VulkanLoader";
	VulkanLoader.SharedLibNames = { "vulkan" };
	VulkanLoader.LibNames = {"vulkan"};
	VulkanLoader.LocatorMethod = ArtifactLocatorMethod::NameHint;
	VulkanLoader.CMakeGenArgs = {
		"BUILD_TESTS=OFF",
		"VULKAN_HEADERS_INSTALL_DIR=" + Filesystem::canonical(VulkanHeadersPath).string()
	};
	VulkanLoader.GitPath = "https://github.com/joeyleavell/Vulkan-Loader.git";
	VulkanLoader.GitLabel = "drac-1";
	VulkanLoader.LabelType = GitLabelType::Branch;
	VulkanLoader.bInstallIncludes = false;

	Dependency Stb;
	Stb.Name = "Stb";
	Stb.IncludesDirectories = {"stb_image/", "stb_truetype"};
	Stb.GitPath = "https://github.com/joeyleavell/Stb.git";
	Stb.LabelType = GitLabelType::None; // Todo: Make this a fork of stb with cmakelists

	std::vector<Dependency> TestDeps = { Bullet3, Glew, Glfw, Json, ShaderConductor, Glm, VulkanHeaders, VulkanLoader, Stb};
	std::vector<Dependency> Targets;

	// Optionally select the target
	if (HasOption(Args, "Target"))
	{
		std::string Target = ParseOption(Args, "Target");

		for(Dependency& Dep : TestDeps)
		{
			if(ToUpper(Dep.Name) == ToUpper(Target))
			{
				Targets.push_back(Dep);
			}
		}
	}
	else
	{
		Targets = TestDeps;
	}


	return BuildDeps(Targets, BuildDir, Tools);
}