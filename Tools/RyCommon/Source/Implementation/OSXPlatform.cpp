#include "Common.h"

#ifdef RYBUILD_OSX

std::string GetPlatformExecutableExt()
{
	return "";
}

std::string GetModulePath()
{
	return "";
}

bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize, char* StdOut, int ErrorBuffSize, char* StdErr)
{
	return false;
}

#endif