#pragma once

#include "Generate/Project.h"
#include <windows.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace RyBuild
{

	// Windows has a constant GUID for c++ project types
	static const std::string CPP_UUID = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942";
	static const std::string SOLUTION_FOLDER_UUID = "2150E333-8FDC-42A3-9474-1A3956D46DE8";
	static const std::string PROJECT_FOLDER_UUID = "66A26720-8FB5-11D2-AA7E-00C04F688DDE";

	class WindowsProjectBase : public Project
	{

	public:

		WindowsProjectBase(const std::string& Name, const std::string& Directory) :
			Project(Name, Directory),
			GUID(CreateGuid())
		{
		}
		
		const std::string& GetGUID() const
		{
			return GUID;
		}

	protected:


		std::string CreateGuid()
		{
			UUID Result;
			ZeroMemory(&Result, sizeof(UUID));
			UuidCreate(&Result);

			CHAR* String = NULL;
			UuidToStringA(&Result, (RPC_CSTR*)&String);

			for (int Char = 0; Char < strlen(String); Char++)
			{
				String[Char] = std::toupper(String[Char]);
			}

			return std::string(String);
		}

		void GlobProjectFiles(std::string ProjectBase, std::string Directory, std::vector<std::string>& Files)
		{
			std::string ProjectCanonDir = std::experimental::filesystem::canonical(ProjectBase).string();
			//	std::string CanonDir = std::experimental::filesystem::canonical(std::string("./") + Project + "\\" + Directory).string();

			std::experimental::filesystem::recursive_directory_iterator Itr(ProjectBase + "\\" + Directory);

			for (std::experimental::filesystem::path Path : Itr)
			{
				if (!std::experimental::filesystem::is_directory(Path))
				{
					std::string FileCanon = std::experimental::filesystem::canonical(Path).string();

					// Remove project canon from beginning
					Files.push_back(FileCanon.substr(ProjectCanonDir.size() + 1));
				}
			}
		}

	private:

		std::string GUID;
		
	};

	
}
