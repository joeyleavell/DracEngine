#include "Common.h"

#ifdef RYBUILD_WINDOWS
#include <windows.h>

std::string GetPlatformExecutableExt()
{
	return ".exe";
}

std::string GetModulePath()
{
	if (ModulePathCache.empty())
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
	for (const std::string Cmd : CmdLineVec)
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
	if (StdOut || StdErr)
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

			if (BytesAvailable > 0)
			{
				unsigned long BytesRead;
				ReadFile(OutputPipeReadHandle, StdOut, OutputBufferSize - 1, &BytesRead, NULL);
			}
		}

		if (StdErr)
		{
			DWORD BytesAvailable;
			PeekNamedPipe(StdErrPipeReadHandle, NULL, 0, NULL, &BytesAvailable, NULL);

			// Initialize error buffer
			memset(StdErr, 0, ErrorBuffSize);

			if (BytesAvailable > 0)
			{
				unsigned long BytesRead;
				ReadFile(StdErrPipeReadHandle, StdErr, ErrorBuffSize - 1, &BytesRead, NULL);
			}

		}
	}

	if (StdErr || StdOut)
	{
		CloseHandle(StdErrPipeReadHandle);
		CloseHandle(OutputPipeReadHandle);
	}

	delete[] CmdLine;

	return bExecSuccessful;
}
#endif