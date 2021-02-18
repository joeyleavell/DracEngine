#pragma once

#include "WindowsProjectBase.h"

namespace RyBuild
{

	class WindowsProjectSolution : public WindowsProjectBase
	{

	public:

		WindowsProjectSolution(const std::string& Name, const std::string& Directory) :
			WindowsProjectBase(Name, Directory)
		{
			
		}

		bool GenerateProject() override;
		
	};
	
}
