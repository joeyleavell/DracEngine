#pragma once

enum BuildConfiguration
{
	/**
	 * Built with debugging symbols and lower optimization settings.
	 */
	DEVELOPMENT,

	/**
	 * Optimized, low file size.
	 */
	SHIPPING
};

enum BuildType
{

	/**
	 * Build each module separately (i.e. dll or so). Create executables only in modules that have an entry point for the specified configuration.
	 */
	MODULAR,

	/**
	 * A single, standalone executable is produced from the build. This means the editor will not be included. This is to create a distributable game.
	 */
	STANDALONE
};

struct BuildPlatform
{
	/** The operating system to target with this build. */
	OSType OS;

	/** The architecture to target with this build */
	ArchitectureType Arch;

	bool operator!=(const BuildPlatform& Other) const
	{
		return OS != Other.OS || Arch != Other.Arch;
	}
	
	bool operator==(const BuildPlatform& Other) const
	{
		return OS == Other.OS && Arch == Other.Arch;
	}
};

struct BuildSettings
{
	/** What build configuration we should use when building the game */
	BuildConfiguration Config;

	/** The build paradigm to use. */
	BuildType Type;

	/** The platform that we are building from. */
	BuildPlatform HostPlatform;

	/** The platform that we want to build to. */
	BuildPlatform TargetPlatform;

	/** The toolset that will be used to perform the build. */
	ToolsetType Toolset;

	/** Whether this is a distribution build. If it is, the editor modules will not be included. */
	bool bDistribute = false;

	/** The directory the build will be placed */
	std::string OutputDirectory = "";

	/** The name of the standalone. This is only relevant if building in standalone mode. */
	std::string StandaloneName = "Build";

	std::string BuildTypeToString() const
	{
		switch (Type)
		{
		case BuildType::MODULAR:
			return "Modular";
		case BuildType::STANDALONE:
			return "Standalone";
		}
		return "None";
	}

	std::string ConfigToString() const
	{
		switch (Config)
		{
		case BuildConfiguration::DEVELOPMENT:
			return "Development";
		case BuildConfiguration::SHIPPING:
			return "Shipping";
		}
		return "None";
	}

	std::string GetTargetPathString() const
	{
		Filesystem::path TargetPath = "";
		
		if (TargetPlatform.Arch == ArchitectureType::X64)
		{
			TargetPath /= "x64";
		}
		else if (TargetPlatform.Arch == ArchitectureType::X86)
		{
			TargetPath /= "x86";
		}
		else if (TargetPlatform.Arch == ArchitectureType::ARM)
		{
			TargetPath /= "Arm";
		}
		else if (TargetPlatform.Arch == ArchitectureType::ARM64)
		{
			TargetPath /= "Arm64";
		}

		if (TargetPlatform.OS == OSType::WINDOWS)
		{
			TargetPath /= "Windows";
		}
		else if (TargetPlatform.OS == OSType::OSX)
		{
			TargetPath /= "OSX";
		}
		else if (TargetPlatform.OS == OSType::LINUX)
		{
			TargetPath /= "Linux";
		}

		if (Toolset == ToolsetType::MSVC)
		{
			TargetPath /= "MSVC";
		}
		else if (Toolset == ToolsetType::GCC)
		{
			if (TargetPlatform.OS == OSType::WINDOWS)
			{
				TargetPath /= "MinGW";
			}
			else
			{
				TargetPath /= "GCC";
			}
		}
		else if (Toolset == ToolsetType::CLANG)
		{
			TargetPath /= "Clang";
		}

		return TargetPath.string();
	}

	/**
	 *
	 * @return A compatibility error message, or empty if no error.
	 */
	std::string CheckCompatability()
	{
		// The following code is spaghetti, but that's the nature of determining compatibility with cross compilation

		if(HostPlatform.OS == OSType::WINDOWS)
		{
			if(TargetPlatform.OS != OSType::WINDOWS)
			{
				return "Cross compilation on Windows is not supported";
			}
		}
		else if(HostPlatform.OS == OSType::LINUX)
		{
			// Linux can only compile with GCC at the moment
			if (Toolset != ToolsetType::GCC)
			{
				return "Linux compilation is only supported with GCC";
			}

			if (TargetPlatform.OS != OSType::LINUX && TargetPlatform.OS != OSType::WINDOWS)
			{
				return "Linux can only cross compile to Windows";
			}
		}
		else if(HostPlatform.OS == OSType::OSX)
		{
			if (Toolset != ToolsetType::GCC)
			{
				return "Mac compilation is only supported with GCC";
			}

			if(TargetPlatform.OS != OSType::OSX)
			{
				return "Cross compilation on Mac is not supported";
			}

		}

		return "";
	}
};