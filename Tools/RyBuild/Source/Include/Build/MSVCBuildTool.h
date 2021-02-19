#pragma once

#include "Module/Module.h"
#include "BuildTool.h"

class MSVCBuildTool : public AbstractBuildTool
{
public:

	MSVCBuildTool(std::string RootDir, BuildSettings Settings);
	
private:

	virtual bool BuildSingleSource(const Module& TheModule, std::string OutputDirectory, std::string SourceFile, std::string& StdOut, std::string& StdErr) override;

	/**
	 * Links a module that has been successfully built.
	 *
	 */
	virtual bool LinkModule(Module& TheModule) override;

	virtual bool LinkStandalone(std::string OutputDirectory, std::string ObjectDirectory, std::string StandaloneName) override;

	/**
	 * Path to the MSVC cl.exe
	 */
	std::string CLPath;

	/**
	 * Path to the MSVC link.exe
	 */
	std::string LinkPath;
	
};