#pragma once

namespace Ry
{

	bool GenerateReflectionCode(std::string Source, 
		std::string Include, 
		std::vector<std::string> Includes,
		std::vector<std::string> Defines,
		std::string& OutGeneratedSource, 
		std::string& ErrorMsg
	);

	 
}