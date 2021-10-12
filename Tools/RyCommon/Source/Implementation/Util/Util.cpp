#include "Util/Util.h"
#include "Common.h"
#include <iostream>
#include <cstdarg>
#include <string>
#include "Windows.h"

static std::string ModulePathCache;
static std::string ModPathCache;
static std::string ModRunPathCache;
static std::string ModEdPathCache;
static std::string BinPathCache;
static std::string IntPathCache;
static std::string ObjPathCache;
static std::string LibPathCache;
static std::string EngineResPathCache;

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

	while(PathMod.has_parent_path() && !bFoundCommonality)
	{
		// Step through the B path
		BaseMod = AbsBase;
		ParentCounter = 0;
		
		while(BaseMod.has_parent_path() && !bFoundCommonality)
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

std::string ParseOption(std::vector<std::string>& Args, std::string Option)
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

std::string GetEngineExternPath(std::string ExternName)
{
	return (Filesystem::path(GetEngineRootDir()) / "External" / ExternName).string();
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

std::string GetRyBuildPath()
{
	std::string RyBuildRoot = Filesystem::path(GetModulePath()).parent_path().string();
	std::string BuildDir;

#ifdef RYBUILD_WINDOWS
	std::string RyBuildModule = (Filesystem::path(RyBuildRoot) / "RyBuild.exe").string();
#elif defined(RYBUILD_LINUX)
	std::string RyBuildModule = (Filesystem::path(RyBuildRoot) / "RyBuild").string();
#endif

	return RyBuildModule;
}

std::string GetRyReflectPath()
{
	std::string RyReflectRoot = Filesystem::path(GetModulePath()).parent_path().string();
	std::string BuildDir;

#ifdef RYBUILD_WINDOWS
	std::string RyReflectModule = (Filesystem::path(RyReflectRoot) / "RyReflect.exe").string();
#elif defined(RYBUILD_LINUX)
	std::string RyReflectModule = (Filesystem::path(RyReflectRoot) / "RyReflect").string();
#endif

	return RyReflectModule;
}

#ifdef RYBUILD_WINDOWS
#include <windows.h>

std::string GetModulePath()
{
	if(ModulePathCache.empty())
	{
		constexpr int MAX_NAME_SIZE = 300;
		char NameSize[MAX_NAME_SIZE];

		HMODULE Mod;
		GetModuleHandleEx(NULL, NULL, &Mod);
		GetModuleFileNameA(Mod, NameSize, MAX_NAME_SIZE);

		ModulePathCache = Filesystem::canonical(std::string(NameSize)).string();
	}

	return ModulePathCache;
}

bool ExecProc(std::string Program, std::vector<std::string>& CmdLineVec, int OutputBufferSize, char* StdOut, int ErrorBuffSize, char* StdErr)
{
	// Copy the command line string into a write-able string
	std::string BuiltCmdLine = Program + " ";
	for(const std::string Cmd : CmdLineVec)
	{
		BuiltCmdLine += Cmd + " ";
	}

	// Copy the built command line into a write-able array for CreateProc
	char* CmdLine = new char[BuiltCmdLine.size() + 1];
	strcpy_s(CmdLine, BuiltCmdLine.size() + 1, BuiltCmdLine.c_str());

	// Create a pipe for the output
	SECURITY_ATTRIBUTES SAttribs;
	SAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);
	SAttribs.bInheritHandle = TRUE;
	SAttribs.lpSecurityDescriptor = NULL;

	// Output pipe handles
	HANDLE OutputPipeReadHandle = NULL;
	HANDLE OutputPipeWriteHandle = NULL;

	// Std error pipe handles
	HANDLE StdErrPipeReadHandle = NULL;
	HANDLE StdErrPipeWriteHandle = NULL;
	
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFOA StartupInfo;

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	// Setup startup info
	StartupInfo.cb = sizeof(StartupInfo);

	// Create output pipe for std out
	if(StdOut || StdErr)
	{
		if (!CreatePipe(&OutputPipeReadHandle, &OutputPipeWriteHandle, &SAttribs, OutputBufferSize))
			return false;

		if (!SetHandleInformation(OutputPipeReadHandle, HANDLE_FLAG_INHERIT, 0))
			return false;

		if (!CreatePipe(&StdErrPipeReadHandle, &StdErrPipeWriteHandle, &SAttribs, OutputBufferSize))
			return false;

		if (!SetHandleInformation(StdErrPipeReadHandle, HANDLE_FLAG_INHERIT, 0))
			return false;

		StartupInfo.hStdOutput = OutputPipeWriteHandle;
		StartupInfo.hStdError = StdErrPipeWriteHandle;
		StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
	}

	bool bExecSuccessful = true;

	//std::cout << CmdLine << std::endl;

	// Start the child process. 
	if (!CreateProcessA(NULL,   // No module name (use command line)
		CmdLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&StartupInfo,            // Pointer to STARTUPINFO structure
		&ProcessInfo)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());

		bExecSuccessful = false;
	}
	else
	{
		// Wait for the compilation process to complete so we're synchronous
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

		DWORD ExitCode;
		BOOL Result = GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode);

		//std::cout << "exit " << std::hex << ExitCode << " " << Result << std::endl;
		//#include "winerror.h"
		
		if (!Result)
		{
			bExecSuccessful = false;
		}
		else
		{
			if (ExitCode != 0)
			{
				bExecSuccessful = false;
			}
		}

		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);

		// Read to the error buffer if it's specified
		if (StdOut)
		{
			DWORD BytesAvailable;
			PeekNamedPipe(OutputPipeReadHandle, NULL, 0, NULL, &BytesAvailable, NULL);

			// Initialize output buffer
			memset(StdOut, 0, OutputBufferSize);

			if(BytesAvailable > 0)
			{
				unsigned long BytesRead;
				ReadFile(OutputPipeReadHandle, StdOut, OutputBufferSize - 1, &BytesRead, NULL);
			}
		}

		if(StdErr)
		{
			DWORD BytesAvailable;
			PeekNamedPipe(StdErrPipeReadHandle, NULL, 0, NULL, &BytesAvailable, NULL);

			// Initialize error buffer
			memset(StdErr, 0, ErrorBuffSize);

			if(BytesAvailable > 0)
			{
				unsigned long BytesRead;
				ReadFile(StdErrPipeReadHandle, StdErr, ErrorBuffSize - 1, &BytesRead, NULL);
			}

		}
	}

	if(StdErr || StdOut)
	{
		CloseHandle(StdErrPipeReadHandle);
		CloseHandle(OutputPipeReadHandle);
	}

	delete[] CmdLine;

	return bExecSuccessful;
}
#elif defined(RYBUILD_LINUX)

#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string.h>

std::string GetModulePath()
{
	if(ModulePathCache.empty())
	{
		constexpr int BUF_SIZE = 1024;
		char Buffer[BUF_SIZE];

		memset(Buffer, 0, sizeof(Buffer));

		if (readlink("/proc/self/exe", Buffer, BUF_SIZE - 1) < 0)
		{
			// Error
			std::cerr << "Error getting /proc/self/exe" << std::endl;
		}

		ModulePathCache = Buffer;
	}
	
	return ModulePathCache;
}

bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize, char* StdOut, int ErrorBuffSize, char* StdErr)
{
	constexpr int ProgramBuffSize = 100;
	char** CommandLine = new char* [CommandLineVec.size() + 2];

	// Copy program into zero'th command line argument
	CommandLine[0] = new char[ProgramBuffSize];

	for (int Param = 1; Param < CommandLineVec.size() + 1; Param++)
	{
		CommandLine[Param] = new char[CommandLineVec[Param - 1].size() + 1];
		strcpy(CommandLine[Param], CommandLineVec[Param - 1].c_str());
	}

	CommandLine[CommandLineVec.size() + 1] = nullptr;
	
	bool bSuccess = true;
	int StdOutPipe[2];
	int StdErrPipe[2];

	if(StdOut || StdErr)
	{
		if (pipe(StdOutPipe) != 0)
		{
			std::cerr << "Failed to create stdout pipe" << std::endl;
			return false;
		}

		if (pipe(StdErrPipe) != 0)
		{
			std::cerr << "Failed to create stderr pipe" << std::endl;
			return false;
		}
	}

	pid_t GccPid = fork();

	if (GccPid < 0)
	{
		// Fork failure
		std::cerr << "Failed to fork() process" << std::endl;
		bSuccess = false;
	}
	else if (GccPid == 0)
	{

		// Close reading ends of pipes in the child since we're only writing
		if(StdOut)
		{
			close(StdOutPipe[0]);

			if (StdOut && dup2(StdOutPipe[1], STDOUT_FILENO) < 0)
			{
				std::cerr << "dup2 failed on stdout, returned " << errno << std::endl;
				exit(1);
			}
		}

		if(StdErr)
		{
			close(StdErrPipe[0]);

			if (StdErr && dup2(StdErrPipe[1], STDERR_FILENO) < 0)
			{
				std::cerr << "dup2 failed on stderr, returned " << errno << std::endl;
				exit(1);
			}
		}

		std::string SearchDirectories[] = {"/usr/bin", "/bin", "/usr/local/bin"};

		char* Environment[] =
		{
			"PATH=/bin:/usr/bin:/usr/local/bin",
			0
		};

		// Try first without using any search directories i.e. give the callee a change to specify the full program path
		execve(Program.c_str(), CommandLine, Environment);

		// Continue to exec through search directories until one works
		for(const std::string SearchDir : SearchDirectories)
		{
			std::string FullProgram = SearchDir + "/" + Program;

			strncpy(CommandLine[0], FullProgram.c_str(), ProgramBuffSize);

			// If this works, program execution will STOP here
			execve(FullProgram.c_str(), CommandLine, Environment);
		}

		// At this point all execs have failed
		std::cout << "Failed to exec program " << Program << std::endl;
		exit(1);
	}
	else
	{

		// Close write ends of pipes on parent proc
		if(StdOut)
		{
			close(StdOutPipe[1]);
		}

		if(StdErr)
		{
			close(StdErrPipe[1]);
		}
		
		// Inside parent proc
		int ExitStatus;

		// We're in the parent proc, wait on child to finish
		if (waitpid(GccPid, &ExitStatus, 0) <= 0)
		{
			std::cerr << "Error waiting for child proc: " << std::endl;
			bSuccess = false;
		}

		delete[] CommandLine;

		// Read from the pipes
		if(StdOut)
		{
			int StdOutRead = read(StdOutPipe[0], StdOut, OutputBufferSize - 1);
			StdOut[StdOutRead] = '\0';
		}

		if(StdErr)
		{
			int StdErrRead = read(StdErrPipe[0], StdErr, ErrorBuffSize - 1);

			StdErr[StdErrRead] = '\0';
		}

		if (WEXITSTATUS(ExitStatus) != 0)
		{
			bSuccess = false;
		}
	}

	// Delete allocated command line params
	// for (int Param = 0; Param < CommandLineVec.size() + 1; Param++)
	// {
	// 	delete CommandLine[Param];
	// }
	
	return bSuccess;
}
#endif

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