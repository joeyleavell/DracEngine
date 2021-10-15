#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <set>
#include "Common.h"

struct FileLOC
{
	std::string FileName;
	int LOC;
};

bool FilterDirectory(Filesystem::path Dir)
{
	std::set<std::string> FilterOutDirs = {"ThirdParty", "Generated", "Intermediate", "Binary", "RapidXML", "Json", "Python", "mingw-std-threads", "Binary", "External"};
	
	std::string DirName = Dir.filename().string();
	if(FilterOutDirs.find(DirName) != FilterOutDirs.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool FilterFile(Filesystem::path File)
{
	std::set<std::string> Exts = {".hpp", ".h", ".cpp", ".hlsl"};
	std::set<std::string> ExcludeFilenames = { "pyconfig", "CMakeCXXCompilerId"};

	if(Exts.find(File.extension().string()) == Exts.end())
	{
		return false;
	}
	
	if(ExcludeFilenames.find(File.stem().string()) != ExcludeFilenames.end())
	{
		return false;
	}

	return true;
}

int main(int ArgC, char** ArgV)
{
	if(ArgC != 2)
	{
		std::cerr << "Incorrect args! Usage: LOCCounter <Root>" << std::endl;
		return 1;
	}

	std::string RootPath = Filesystem::canonical(ArgV[1]).string();

	std::vector<std::string> SourceFilesToCount;
	std::vector<std::string> HeaderFilesToCount;
	std::vector<std::string> AllFilesToCount;

	std::vector<std::string> DirectoriesToIterate;
	DirectoriesToIterate.push_back(RootPath);

	while(!DirectoriesToIterate.empty())
	{
		std::vector<std::string> FoundDirectories;

		std::string NextDir = DirectoriesToIterate[0];

		// Remove that first element from the vector
		DirectoriesToIterate.erase(DirectoriesToIterate.begin());
		
		Filesystem::directory_iterator NewDirectoryItr(NextDir);
		for(Filesystem::path Path : NewDirectoryItr)
		{
			if(Filesystem::is_directory(Path))
			{
				// Do directory check

				// Don't count anything in third party directories
				if(FilterDirectory(Path))
				{
					FoundDirectories.push_back(Path.string());
				}
			}
			else
			{
				if (!FilterFile(Path))
				{
					continue;
				}

				// Do source file check
				if(Path.extension() == ".h" || Path.extension() == ".hpp")
				{
					HeaderFilesToCount.push_back(Path.string());
					AllFilesToCount.push_back(Path.string());
				}
				else if(Path.extension() == ".cpp")
				{
					SourceFilesToCount.push_back(Path.string());
					AllFilesToCount.push_back(Path.string());
				}
			}
		}

		for(const std::string& NewDir : FoundDirectories)
		{
			DirectoriesToIterate.push_back(NewDir);
		}
	}

	int HeaderCount = 0;
	int SourceCount = 0;
	std::vector<FileLOC> LOCs;

	// Do count
	for(std::string& FoundFile : SourceFilesToCount)
	{
		std::ifstream ReadStream(FoundFile);
		std::string OutString;

		FileLOC LOC;
		LOC.FileName = FoundFile;
		LOC.LOC = 0;
		
		while(std::getline(ReadStream, OutString))
		{
			SourceCount++;
			LOC.LOC++;
		}

		LOCs.push_back(LOC);
	}

	for (std::string& FoundFile : HeaderFilesToCount)
	{
		std::ifstream ReadStream(FoundFile);
		std::string OutString;

		FileLOC LOC;
		LOC.FileName = FoundFile;
		LOC.LOC = 0;

		while (std::getline(ReadStream, OutString))
		{
			HeaderCount++;
			LOC.LOC++;
		}

		LOCs.push_back(LOC);
	}

	// Sort all files by LOC
	std::sort(LOCs.begin(), LOCs.end(), [](const FileLOC& A, const FileLOC& B) {
		return A.LOC < B.LOC;
	});

	int MedianLOC = 0;
	if(!LOCs.empty())
	{
		if (LOCs.size() % 2 == 0)
		{
			MedianLOC = (LOCs[LOCs.size() / 2].LOC + LOCs[LOCs.size() / 2 - 1].LOC) / 2.0f;
		}
		else
		{
			MedianLOC = LOCs[LOCs.size() / 2].LOC;
		}
	}

	std::cout << "Total LOC: " << (HeaderCount + SourceCount) << std::endl;
	std::cout << "Header only: " << (HeaderCount) << std::endl;
	std::cout << "Source only: " << (SourceCount) << std::endl;
	std::cout << "Header % of total: " << (HeaderCount / ((float) HeaderCount + SourceCount)) << std::endl;
	std::cout << "Source % of total: " << (SourceCount / ((float)HeaderCount + SourceCount)) << std::endl;
	std::cout << "Avg Header LOC: " << (HeaderCount / (float) HeaderFilesToCount.size()) << std::endl;
	std::cout << "Avg Source LOC: " << (SourceCount / (float)SourceFilesToCount.size()) << std::endl;
	std::cout << "Median LOC: " << MedianLOC << std::endl;

	std::cout << std::endl;

	// Print out the top N files
	constexpr int N = 50;
	std::cout << "Top " << N << " files by size: " << std::endl;
	for(int I = 0; I < N && I < LOCs.size(); I++)
	{
		int _I = LOCs.size() - I - 1;
		std::cout << Filesystem::path(LOCs[_I].FileName).filename().string() << ", " << LOCs[_I].LOC << " LOC, % of total=" << ((float)LOCs[_I].LOC / (SourceCount + HeaderCount)) << std::endl;
	}
	
	return 0;
}