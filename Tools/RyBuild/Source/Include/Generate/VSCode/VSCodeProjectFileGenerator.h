#pragma once

#include <string>

bool GenerateEngineProjectFiles_VSCode(std::string EngineRootPath, std::string Compiler);
bool GenerateGameProjectFiles_VSCode(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler);
