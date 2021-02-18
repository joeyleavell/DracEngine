#pragma once
#include <string>

class CommandBuilder
{

public:

	void AddOption(std::string Option);
	void AddKey(std::string Option);
	void AddRawString(std::string String);
	void AddQuotedString(std::string String);
	void AddSpace();

	std::string& ToString();

private:

	std::string Result;
	
};
