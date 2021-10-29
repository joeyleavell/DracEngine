#include "Util/Util.h"
#include "Common.h"
#include <iostream>
#include <cstdarg>
#include <string>

void IncorrectUsage(std::string Usage)
{
	std::cerr << "Incorrect usage. Correct usage: " << Usage << std::endl;
}

std::string ToUpper(std::string Word)
{
	std::string Caps;
	for (char ModuleChar : Word)
	{
		Caps += std::toupper(ModuleChar);
	}

	return Caps;
}

/**
 * Checks if A is the parent of B 
 */
bool IsParentOf(Filesystem::path A, Filesystem::path B)
{
	Filesystem::path BaseAbs = Filesystem::absolute(A);
	Filesystem::path OtherAbs = Filesystem::absolute(B);

	// Todo: check if this removes trailing path separator ?
	BaseAbs.replace_filename(BaseAbs.filename());
	OtherAbs.replace_filename(OtherAbs.filename());

	while(OtherAbs.has_parent_path())
	{
		if(OtherAbs == BaseAbs)
		{
			return true;
		}
		
		OtherAbs = OtherAbs.parent_path();
	}

	return false;
}

Filesystem::path PathRelativeTo_UnRelated(Filesystem::path Base, Filesystem::path Path)
{
	Filesystem::path AbsBase = Filesystem::absolute(Base);
	Filesystem::path AbsPath = Filesystem::absolute(Path);

	Filesystem::path BaseMod = AbsBase;
	Filesystem::path PathMod = AbsPath;

	// The part accumulated from the unrelated path
	Filesystem::path AccumPath;
	int ParentCounter = 0;
	bool bFoundCommonality = false;

	while((PathMod.has_parent_path() && PathMod.has_relative_path()) && !bFoundCommonality)
	{
		// Step through the B path
		BaseMod = AbsBase;
		ParentCounter = 0;
		
		while((BaseMod.has_parent_path() && BaseMod.has_relative_path()) && !bFoundCommonality)
		{
			if (BaseMod == PathMod)
			{
				bFoundCommonality = true;
			}
			else
			{
				ParentCounter++;
				BaseMod = BaseMod.parent_path();
			}

		}

		if(!bFoundCommonality)
		{
			// Accumulate this part and step up
			if(AccumPath.empty())
				AccumPath = PathMod.filename();
			else
				AccumPath = PathMod.filename() / AccumPath;

			PathMod = PathMod.parent_path();
		}
		
	}

	if(ParentCounter == 1)
	{
		AccumPath = "." / AccumPath;
		ParentCounter--;
	}
	
	// Add the parent path markers
	// This goes up until we have a common base, then we can go until we reached the other path
	for(int ParentUp = 0; ParentUp < ParentCounter; ParentUp++)
	{
		AccumPath = ".." / AccumPath;
	}

	return AccumPath;

}

Filesystem::path PathRelativeTo(Filesystem::path Base, Filesystem::path Other)
{
	return PathRelativeTo_UnRelated(Base, Other);
}

bool HasOption(std::vector<std::string>& Args, std::string Option)
{
	for (std::string& Opt : Args)
	{
		if (Opt.find(Option) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

void ParseMultiOption(std::vector<std::string>& OutValues, std::vector<std::string>& Args, std::string Option)
{
	for (std::string& Opt : Args)
	{
		if (Opt.find(Option) != std::string::npos)
		{
			int SepIndex = Opt.find("=");

			std::string Res = Opt.substr(SepIndex + 1);
			OutValues.push_back(Res);
		}
	}
}

std::string ParseUniqueOption(std::vector<std::string>& Args, std::string Option)
{
	for (std::string& Opt : Args)
	{
		if (Opt.find(Option) != std::string::npos)
		{
			int SepIndex = Opt.find("=");

			return Opt.substr(SepIndex + 1);
		}
	}

	return "";
}

std::string FindNonOption(std::vector<std::string>& Options)
{
	for (const std::string& Opt : Options)
	{
		if (Opt.size() > 0 && Opt[0] != '-')
		{
			return Opt;
		}
	}

	return "";
}

std::string FullPath(std::string Path)
{
	if(!Filesystem::exists(Path))
	{
		Filesystem::create_directories(Path);
	}

	return Filesystem::canonical(Path).string();
}

void FormatString(char* OutBuffer, int OutBufferSize, const char* Format, ...)
{
	va_list VarArgs;
	va_start(VarArgs, Format);

#ifdef RYBUILD_WINDOWS
	vsprintf_s(OutBuffer, OutBufferSize, Format, VarArgs);
#else
	vsprintf(OutBuffer, Format, VarArgs);
#endif

	va_end(VarArgs);
}

std::string GetEngineRootDir()
{
	std::string ModulePath = GetModulePath();
	std::string EngineRootPath = Filesystem::path(ModulePath).parent_path().parent_path().parent_path().string();

	return EngineRootPath;
}

std::string GetEngineModulesDir()
{
	if (ModPathCache.empty())
	{
		ModPathCache = (Filesystem::path(GetEngineRootDir()) / "Modules").string();
	}

	return ModPathCache;
}

std::string GetEngineRuntimeModulesDir()
{
	if (ModRunPathCache.empty())
	{
		ModRunPathCache = (Filesystem::path(GetEngineModulesDir()) / "Runtime").string();
	}

	return ModRunPathCache;
}

std::string GetEngineEditorModulesDir()
{
	if (ModEdPathCache.empty())
	{
		ModEdPathCache = (Filesystem::path(GetEngineModulesDir()) / "Editor").string();
	}

	return ModEdPathCache;
}

std::string GetEngineExternRoot()
{
	return (Filesystem::path(GetEngineRootDir()) / "External").string();
}

std::string GetEngineExternPath(std::string ExternName)
{
	return (Filesystem::path(GetEngineRootDir()) / "External" / ExternName).string();
}

std::string GetEditorExecutable()
{
	return (Filesystem::path(GetEngineRootDir()) / "Binary" / "RyRuntime-EditorMain").string();
}

std::string GetEngineBinaryDir()
{
	if(BinPathCache.empty())
	{
		BinPathCache = (Filesystem::path(GetEngineRootDir()) / "Binary").string();
	}

	return BinPathCache;
}

std::string GetEngineExecutable()
{
	std::string EngineBin = GetEngineBinaryDir();

	return (Filesystem::path(EngineBin) / "RyRuntime-EditorMain.exe").string();
}

std::string GetEngineIntermediateDir()
{
	if(IntPathCache.empty())
	{
		IntPathCache = (Filesystem::path(GetEngineRootDir()) / "Intermediate").string();
	}

	return IntPathCache;
}

std::string GetEngineObjectDir()
{
	if(ObjPathCache.empty())
	{
		ObjPathCache = (Filesystem::path(GetEngineRootDir()) / "Intermediate" / "Object").string();
	}

	return ObjPathCache;
}

std::string GetEngineLibraryDir()
{
	if(LibPathCache.empty())
	{
		LibPathCache = (Filesystem::path(GetEngineRootDir()) / "Intermediate" / "Libraries").string();
	}

	return LibPathCache;
}

std::string GetEngineResourcesDir()
{
	if (EngineResPathCache.empty())
	{
		EngineResPathCache = (Filesystem::path(GetEngineRootDir()) / "Resources").string();
	}

	return EngineResPathCache;
}

std::string GetToolsBinariesRoot()
{
	return Filesystem::path(GetModulePath()).parent_path().string();
}

std::string GetRyBuildPath()
{
	std::string RyBuildRoot = Filesystem::path(GetModulePath()).parent_path().string();
	std::string RyBuildModule = (Filesystem::path(RyBuildRoot) / ("RyBuild" + GetPlatformExecutableExt())).string();

	return RyBuildModule;
}

std::string GetRyReflectPath()
{
	std::string RyReflectRoot = Filesystem::path(GetModulePath()).parent_path().string();
	std::string RyReflectModule = (Filesystem::path(RyReflectRoot) / ("RyReflect" + GetPlatformExecutableExt())).string();

	return RyReflectModule;
}

std::string GetTimestamp(std::string Format)
{
	char Buffer[1024 * 4];

	time_t Now = time(0);
	struct tm AsTime;

#ifdef MSVC_COMPILER
	localtime_s(&AsTime, &Now);
#else
	AsTime = *localtime(&Now);
#endif

	strftime(Buffer, 1024 * 4, Format.c_str(), &AsTime);

	return Buffer;
}