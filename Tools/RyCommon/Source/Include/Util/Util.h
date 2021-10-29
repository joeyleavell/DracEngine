#pragma once

#include <string>
#include <vector>
#include "Common.h"

void IncorrectUsage(std::string Usage);

// Options parsing
bool HasOption(std::vector<std::string>& Args, std::string Option);
std::string ParseUniqueOption(std::vector<std::string>& Args, std::string Option);
void ParseMultiOption(std::vector<std::string>& OutValues, std::vector<std::string>& Args, std::string Option);

std::string FindNonOption(std::vector<std::string>& Options);

std::string ReadFileAsString(std::string Path);

// File paths utils
std::string FullPath(std::string Path);
std::string ToUpper(std::string Word);
Filesystem::path PathRelativeTo(Filesystem::path Base, Filesystem::path Other);

void FormatString(char* OutBuffer, int OutBufferSize, const char* Format, ...);

// Engine directories
std::string GetEngineRootDir();
std::string GetEngineModulesDir();
std::string GetEngineRuntimeModulesDir();
std::string GetEngineEditorModulesDir();
std::string GetEngineExternPath(std::string ExternName);
std::string GetEngineExternRoot();
std::string GetEngineBinaryDir();
std::string GetEditorExecutable();
std::string GetEngineExecutable();
std::string GetEngineIntermediateDir();
std::string GetEngineObjectDir();
std::string GetEngineLibraryDir();
std::string GetEngineResourcesDir();

// Executable tools paths
std::string GetRyBuildPath();
std::string GetRyReflectPath();
std::string GetToolsBinariesRoot();

std::string GetTimestamp(std::string Format);