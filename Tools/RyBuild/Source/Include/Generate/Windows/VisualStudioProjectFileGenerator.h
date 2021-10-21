#pragma once

#include <string>

bool GenerateEngineProjectFiles_VisualStudio(std::string EngineRootPath, std::string Compiler);
bool GenerateGameProjectFiles_VisualStudio(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler);
