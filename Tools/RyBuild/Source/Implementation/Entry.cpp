#include <iostream>
#include "Json/json.hpp"
#include "Common.h"
#include "Util/Util.h"
#include "Build/BuildTool.h"
#include "Deps/BuildDeps.h"
#include "Generate/GenerateModule.h"
#include "Generate/GenerateGame.h"
#include "Generate/ProjectFileGenerator.h"

enum OutputType
{
	EXE, DLL, LIB
};

void GenerateCmd(std::vector<std::string>& Args)
{

	if(Args.size() < 2)
	{
		IncorrectUsage("generate <Cmd> ...");
		return;
	}

	std::string GenerateCmd = Args[1];

	if(GenerateCmd == "ProjectFiles")
	{
		GenerateProjectFilesCmd(Args);
	}
	else if(GenerateCmd == "Module")
	{
		GenerateModuleCmd(Args);
	}
	else if(GenerateCmd == "Game")
	{
		GenerateGameCmd(Args);
	}
	else
	{
		std::cerr << "Generate command not recognized. Must be one of ProjectFiles, Module, or Game." << std::endl;
	}
	
}

void PrintManPages()
{
	// Todo: Implement	
}

int main(int ArgCount, char** ArgValues)
{
	
	if (ArgCount < 2)
	{
		std::cerr << "RyBuild requires you specify a command: generate, build, rebuild, clean, deps" << std::endl;
		return 1;
	}

	std::vector<std::string> Args;

	std::string SubprogramCmd = ArgValues[1];
	for(int CmdIndex = 1; CmdIndex < ArgCount; CmdIndex++)
	{
		std::string Arg = ArgValues[CmdIndex];

		// Check if arg is verbose. This argument is pretty common, so store it globally.
		if(Arg.find("verbose") != std::string::npos)
		{
			bIsVerbose = true;
		}
		
		// Copy over arguments not including first
		Args.push_back(Arg);
	}

	if(SubprogramCmd == "generate")
	{
		GenerateCmd(Args);
	}
	else if(SubprogramCmd == "build")
	{
		if(!BuildCmd(Args))
		{
			return 1;
		}
	}
	else if (SubprogramCmd == "rebuild")
	{
		if(!RebuildCmd(Args))
		{
			return 1;
		}
	}
	else if(SubprogramCmd == "clean")
	{
		if(!CleanCmd(Args))
		{
			return 1;
		}
	}
	else if(SubprogramCmd == "deps")
	{
		if (!BuildDepsCmd(Args))
		{
			return 1;
		}
	}
	else
	{
		std::cerr << "Command not recognized" << std::endl;
	}

	return 0;
}