#include "Generate/Xcode/XCodeProjectFileGenerator.h"
#include <iostream>
#include "Common.h"
#include <fstream>
#include <cstdint>
#include <random>
#include <sstream>
#include "Util/Util.h"

constexpr char* ProjectName = "AryzeEngine";

std::random_device Rand;
std::default_random_engine Engine(Rand());
std::uniform_int_distribution<uint64_t> Dist(0, UINT64_MAX);

struct XCodeData
{
	Filesystem::path RootAbs;
	Filesystem::path XCodeProjDir;
	Filesystem::path PbxFile;
};

class XCodeUUID
{

public:

	static XCodeUUID Create()
	{
		XCodeUUID Result;
		Result.Lower64 = Dist(Engine);
		Result.Upper32 = Dist(Engine) & 0xFFFFFFFF;

		return Result;
	}

	std::string ToString()
	{
		std::ostringstream Out;
		Out << std::hex << Upper32 << Lower64;

		// Convert to upper case
		return ToUpper(Out.str());
	}

private:

	XCodeUUID() {};

	uint32_t Upper32;
	uint64_t Lower64;
};

class PbxWriter
{

public:

	PbxWriter(): Indentation(0) {};

	void Write(std::string Output)
	{
		OutPbx.open(Output);
		
		// UTF header
		WriteLine("// !$*UTF8*$!");
		WriteLineAddIndent("{");
		{
			// Archive		
			WriteLine("archiveVersion = 1;");

			WriteLine("classes = {");
			WriteLine("}");

			WriteLine("objectVersion = 55;");

			WriteLineAddIndent("objects = {");
			{
				// Write out file references

				// Write out groups

				// Write out native targets

				// Write out project

				// Write out build phase

				// Write out XC build configuration

				// Write out configuration list

				// Write out root object (project)
			}
			WriteLineRemoveIndent("};");

		}				
		// Write out root object
		WriteLineRemoveIndent("}");

		OutPbx.close();
	}

private:

	void AddIndent()
	{
		Indentation++;
	}

	void RemoveIndent()
	{
		Indentation--;
	}

	void WriteLine(std::string Line)
	{
		for (int Indent = 0; Indent < Indentation; Indent++)
			OutPbx << "\t";

		OutPbx << Line << std::endl;
	}

	void WriteLineIgnoreIndents(std::string Line)
	{
		OutPbx << Line << std::endl;
	}

	void WriteLineAddIndent(std::string Line)
	{
		WriteLine(Line);
		AddIndent();
	}

	void WriteLineRemoveIndent(std::string Line)
	{
		RemoveIndent();
		WriteLine(Line);
	}

	std::ofstream OutPbx;
	int Indentation;
	
};

void CreatePbxProj(XCodeData& Data)
{
	Data.PbxFile = Data.XCodeProjDir / "project.pbxproj";

	PbxWriter Writer;

	XCodeUUID Test = XCodeUUID::Create();
	std::cout << "UUID: " << Test.ToString() << std::endl;

	Writer.Begin(Data.PbxFile.string());
	{
		
	}
	Writer.End();
}

bool GenerateEngineProjectFiles_XCode(std::string EngineRootPath, std::string Compiler)
{
	XCodeData Data;
	Data.RootAbs = Filesystem::canonical(EngineRootPath);
	Data.XCodeProjDir = Data.RootAbs / (ProjectName + std::string(".xcodeproj"));

	// Create .xcodeproj directory
	Filesystem::create_directories(Data.XCodeProjDir);

	// Create pbxproj
	CreatePbxProj(Data);

	return false;
}

bool GenerateGameProjectFiles_XCode(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler)
{
	return false;
}