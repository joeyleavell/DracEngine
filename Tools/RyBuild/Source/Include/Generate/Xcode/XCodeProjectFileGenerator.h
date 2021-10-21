#pragma once
#include <string>

bool GenerateEngineProjectFiles_XCode(std::string EngineRootPath, std::string Compiler);
bool GenerateGameProjectFiles_XCode(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler);