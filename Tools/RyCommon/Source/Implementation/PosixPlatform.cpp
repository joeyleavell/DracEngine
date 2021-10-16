#include "Common.h"

#if defined(RBUILD_HOST_OS_LINUX) || defined(RBUILD_HOST_OS_OSX)

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string.h>

std::string GetPlatformExecutableExt()
{
	return "";
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

	if (StdOut || StdErr)
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
		if (StdOut)
		{
			close(StdOutPipe[0]);

			if (StdOut && dup2(StdOutPipe[1], STDOUT_FILENO) < 0)
			{
				std::cerr << "dup2 failed on stdout, returned " << errno << std::endl;
				exit(1);
			}
		}

		if (StdErr)
		{
			close(StdErrPipe[0]);

			if (StdErr && dup2(StdErrPipe[1], STDERR_FILENO) < 0)
			{
				std::cerr << "dup2 failed on stderr, returned " << errno << std::endl;
				exit(1);
			}
		}

		std::string SearchDirectories[] = { "/opt/homebrew/bin", "/usr/bin", "/bin", "/usr/local/bin" };

		char* Environment[] =
		{
			"PATH=/opt/homebrew/bin:/bin:/usr/bin:/usr/local/bin",
			0
		};

		// Try first without using any search directories i.e. give the callee a change to specify the full program path
		execve(Program.c_str(), CommandLine, Environment);

		// Continue to exec through search directories until one works
		for (const std::string SearchDir : SearchDirectories)
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
		if (StdOut)
		{
			close(StdOutPipe[1]);
		}

		if (StdErr)
		{
			close(StdErrPipe[1]);
		}

		// Inside parent proc
		int ExitStatus;

		waitpid(GccPid, &ExitStatus, 0);
		// We're in the parent proc, wait on child to finish
		//if (Res <= 0)
		{
	//		std::cerr << "Error waiting for child proc: " << Res << " (exit status=" << ExitStatus << ")" << std::endl;
	//		bSuccess = false;
		}

		delete[] CommandLine;

		// Read from the pipes
		if (StdOut)
		{
			int StdOutRead = read(StdOutPipe[0], StdOut, OutputBufferSize - 1);
			StdOut[StdOutRead] = '\0';
		}

		if (StdErr)
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