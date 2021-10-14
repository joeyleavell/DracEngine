#pragma once

#include <string>
#include <vector>
#include "Common.h"

void IncorrectUsage(std::string Usage);
bool HasOption(std::vector<std::string>& Args, std::string Option);
std::string ParseOption(std::vector<std::string>& Args, std::string Option);
std::string FindNonOption(std::vector<std::string>& Options);
std::string FullPath(std::string Path);
std::string ToUpper(std::string Word);
Filesystem::path PathRelativeTo(Filesystem::path Base, Filesystem::path Other);

void FormatString(char* OutBuffer, int OutBufferSize, const char* Format, ...);

std::string GetModulePath();
std::string GetEngineRootDir();
std::string GetEngineModulesDir();
std::string GetEngineRuntimeModulesDir();
std::string GetEngineEditorModulesDir();
std::string GetEngineExternPath(std::string ExternName);

std::string GetEngineBinaryDir();
std::string GetEngineExecutable();
std::string GetEngineIntermediateDir();
std::string GetEngineObjectDir();
std::string GetEngineLibraryDir();
std::string GetEngineResourcesDir();

std::string GetRyBuildPath();
std::string GetRyReflectPath();


std::string GetTimestamp(std::string Format);