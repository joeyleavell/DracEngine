#pragma once

enum BuildToolset
{
	MSVC,
	GCC,
	CLANG
};

enum BuildConfiguration
{
	/**
	 * Built with debugging symbols and lower optimization settings.
	 */
	Development,

	/***
	 * Optimized, low file size.
	 */
	 Shipping
};

enum BuildType
{

	/**
	 * Build each module separately (i.e. dll or so). Create executables only in modules that have an entry point for the specified configuration.
	 */
	Modular,

	/**
	 * A single, standalone executable is produced from the build. This means the editor will not be included. This is to create a distributable game.
	 */
	Standalone
};

enum TargetArchitecture
{
	/** Target x86 (32bit) instruction set */
	x86,

	/** Target x86_64 (64bit) instruction set */
	x86_64,

	/** Target arm instruction set */
	Arm
};

enum TargetOS
{
	/** Target the Microsoft Windows operating system */
	Windows,

	/** Target the Linux operating system */
	Linux,

	/** Target the MAC operating system */
	Mac
};

struct BuildPlatform
{
	/** The operating system to target with this build. */
	TargetOS OS;

	/** The architecture to target with this build */
	TargetArchitecture Arch;

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
	BuildToolset Toolset;

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
		case BuildType::Modular:
			return "Modular";
		case BuildType::Standalone:
			return "Standalone";
		}
		return "None";
	}

	std::string ConfigToString() const
	{
		switch (Config)
		{
		case BuildConfiguration::Development:
			return "Development";
		case BuildConfiguration::Shipping:
			return "Shipping";
		}
		return "None";
	}

	std::string TargetOSToString() const
	{
		switch(TargetPlatform.OS)
		{
		case TargetOS::Mac:
			return "Mac";
		case TargetOS::Windows:
			return "Windows";
		case TargetOS::Linux:
			return "Linux";
		}
		return "None";
	}

	std::string TargetArchToString() const
	{
		switch (TargetPlatform.Arch)
		{
		case TargetArchitecture::x86_64:
			return "x86_64";
		case TargetArchitecture::x86:
			return "x86";
		case TargetArchitecture::Arm:
			return "Arm";
		}
		return "None";
	}

	std::string ToolsetToString() const
	{
		switch (Toolset)
		{
		case BuildToolset::CLANG:
			return "Clang";
		case BuildToolset::GCC:
			return "GCC";
		case BuildToolset::MSVC:
			return "MSVC";
		}
		return "None";
	}


	std::string GetTargetPathString() const
	{
		Filesystem::path TargetPath = "";
		
		if (TargetPlatform.Arch == TargetArchitecture::x86_64)
		{
			TargetPath /= "x64";
		}
		else if (TargetPlatform.Arch == TargetArchitecture::x86)
		{
			TargetPath /= "x86";
		}
		else if (TargetPlatform.Arch == TargetArchitecture::Arm)
		{
			TargetPath /= "Arm";
		}

		if (TargetPlatform.OS == TargetOS::Windows)
		{
			TargetPath /= "Windows";
		}
		else if (TargetPlatform.OS == TargetOS::Mac)
		{
			TargetPath /= "Mac";
		}
		else if (TargetPlatform.OS == TargetOS::Linux)
		{
			TargetPath /= "Linux";
		}

		if (Toolset == BuildToolset::MSVC)
		{
			TargetPath /= "MSVC";
		}
		else if (Toolset == BuildToolset::GCC)
		{
			if (TargetPlatform.OS == TargetOS::Windows)
			{
				TargetPath /= "MinGW";
			}
			else
			{
				TargetPath /= "GCC";
			}
		}
		else if (Toolset == BuildToolset::CLANG)
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
		if (TargetPlatform.Arch != TargetArchitecture::x86_64)
		{
			return "Only x86_64 targets are supported";
		}

		if(HostPlatform.OS == TargetOS::Windows)
		{
			if(TargetPlatform.OS != TargetOS::Windows)
			{
				return "Cross compilation on Windows is not supported";
			}
		}
		else if(HostPlatform.OS == TargetOS::Linux)
		{
			// Linux can only compile with GCC at the moment
			if (Toolset != BuildToolset::GCC)
			{
				return "Linux compilation is only supported with GCC";
			}

			if (TargetPlatform.OS != TargetOS::Linux && TargetPlatform.OS != TargetOS::Windows)
			{
				return "Linux can only cross compile to Windows";
			}
		}
		else
		{
			// No other platforms are supported at the moment
			return "Only Windows and Linux hosts are supported at the moment";
		}

		return "";
	}
};