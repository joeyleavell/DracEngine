#pragma once

#include <string>
#include <vector>

extern std::string ModulePathCache;
extern std::string ModPathCache;
extern std::string ModRunPathCache;
extern std::string ModEdPathCache;
extern std::string BinPathCache;
extern std::string IntPathCache;
extern std::string ObjPathCache;
extern std::string LibPathCache;
extern std::string EngineResPathCache;

bool ExecProc(std::string Program, std::vector<std::string>& CommandLineVec, int OutputBufferSize = 0, char* StdOut = nullptr, int ErrorBuffSize = 0, char* StdErr = nullptr);