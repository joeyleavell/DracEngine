#include "Common.h"

#if defined(RBUILD_HOST_OS_LINUX) || defined(RBUILD_HOST_OS_OSX)

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string.h>
#include <signal.h>

std::string GetPlatformExecutableExt()
{
	return "";
}

bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize, char* StdOut, int ErrorBuffSize, char* StdErr)
{
	constexpr int ProgramBuffSize = 100;
	char** CommandLine = new char* [CommandLineVec.size() + 2];
	bool bSuccess = false;
	int StdOutPipe[2]; // Output pipe
	int StdErrPipe[2]; // Error pipe

	// Copy program into zero'th command line argument
	CommandLine[0] = new char[ProgramBuffSize];

	for (int Param = 1; Param < CommandLineVec.size() + 1; Param++)
	{
		CommandLine[Param] = new char[CommandLineVec[Param - 1].size() + 1];
		strcpy(CommandLine[Param], CommandLineVec[Param - 1].c_str());
	}

	CommandLine[CommandLineVec.size() + 1] = nullptr;

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
		// ---- Fork failure ----

		std::cerr << "Failed to fork() process" << std::endl;
	}
	else if (GccPid == 0)
	{
		// ---- Inside child proc ----

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

		const char* PathString = "PATH=/opt/homebrew/bin:/usr/local/bin:/bin:/usr/bin";
		char** Environment = (char**) malloc(sizeof(char*) * 2);
		Environment[0] = (char*) malloc(sizeof(char) * sizeof(PathString));
		Environment[1] = NULL;

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
		// ---- Inside parent proc ----

		int ExitStatus;

		if (StdOut)
			close(StdOutPipe[1]);
		if (StdErr)
			close(StdErrPipe[1]);

		// Sleep for 10 milliseconds
	    struct timespec TimeSpec;
		TimeSpec.tv_sec = 0;
		TimeSpec.tv_nsec = 10 * 1e6;
		
		// Wait until process is done, don't hang to avoid getting a system interrupt
		while(waitpid(GccPid, &ExitStatus, WNOHANG) == 0)
		{
			nanosleep(&TimeSpec, &TimeSpec);
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

		if(WIFEXITED(ExitStatus) && WEXITSTATUS(ExitStatus) == 0)
		{
			bSuccess = true;
		}

	}

	return bSuccess;
}
#endif