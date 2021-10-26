#pragma once

#ifdef RBUILD_HOST_OS_WINDOWS

#include "Generate/Windows/WindowsProjectBase.h"

namespace RyBuild
{

	/**
	 * Represents a windows solution folder.
	 */
	class WindowsProjectFolder : public WindowsProjectBase
	{

	public:

		WindowsProjectFolder(const std::string& Name, const std::string& Directory) :
			WindowsProjectBase(Name, Directory)
		{
		}

		bool GenerateProject() override
		{
			// Create a folder

			return std::experimental::filesystem::create_directories(GetAbsoluteProjectPath());
		}
		
	};
	
}

#endif