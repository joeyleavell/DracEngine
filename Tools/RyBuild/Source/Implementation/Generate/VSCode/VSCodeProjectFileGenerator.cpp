#include "Generate/VSCode/VSCodeProjectFileGenerator.h"
#include "Common.h"
#include "Util/Util.h"
#include <fstream>
#include "Build/Module/Module.h"
#include <Json/json.hpp>
#include <unordered_set>
#include <iostream>

using namespace nlohmann;

/*
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${default}",
                "${workspaceFolder}/Tools/RyCommon/Source/Include",
                "${workspaceFolder}/Tools/RyBuild/Source/Include",
                "${workspaceFolder}/Modules/Editor/EditorMain/Include"
            ],
            "compilerPath": "/opt/homebrew/bin/gcc-11",
            "cStandard": "gnu17",
            "cppStandard": "gnu++17",
            "intelliSenseMode": "macos-gcc-arm64"
        }
    ],
    "version": 4
}
*/

bool GenerateEngineProjectFiles_VSCode(std::string EngineRootPath, std::string Compiler)
{
    // Create tasks.json
    Filesystem::path VsCodePath = Filesystem::path(EngineRootPath) / ".vscode";
    Filesystem::path CppConfigPath  = VsCodePath / "c_cpp_properties.json";

    Filesystem::create_directories(VsCodePath);

    std::string RyBuildPath = GetRyBuildPath();
    // Get RyBuild relative to workspace root
    std::string RyToolsBinaryWorkspaceRelative = "${workspaceFolder}/" + PathRelativeTo(GetEngineRootDir(), GetToolsBinariesRoot()).string();
    std::string RyBuildPathWorkspaceRelative = "${workspaceFolder}/" + PathRelativeTo(GetEngineRootDir(), RyBuildPath).string();
    std::string RyBuildModulesPathWorkspaceRelative  = "${workspaceFolder}/" + PathRelativeTo(GetEngineRootDir(), GetEngineModulesDir()).string();

    // Discover all possible include search paths
    std::vector<Module*> Modules;
    DiscoverModules(GetEngineModulesDir(), Modules);

    std::unordered_set<std::string> IncludePaths;
		for(auto& Mod : Modules)
		{
			IncludePaths.insert(Mod->GetIncludeDir());
			IncludePaths.insert(Mod->GetGeneratedDir());
		}

    // Add extern includes
    Filesystem::directory_iterator ExternItr (GetEngineExternRoot());
    for(auto& Extern : ExternItr)
    {
      if(Extern.is_directory())
      {
        IncludePaths.insert((Extern.path() / "Include").string());
      }
    }

    // Generate tasks.json
    {
      Filesystem::path TasksJsonPath  = VsCodePath / "tasks.json";
      json RootTasksJson;
      RootTasksJson["version"] = "2.0.0";

      json BuildTaskJson = {
        {"type", "shell"},
        {"label", "Build Engine"},
        {"command", RyBuildPathWorkspaceRelative},
        {"args", json::array({
          "build",
          RyBuildModulesPathWorkspaceRelative
        })},
        {"options", {
          {"cwd", RyToolsBinaryWorkspaceRelative}
        }},
        {"problemMatcher", json::array({"$gcc"})},
        {"group", {
          {"kind", "build"},
          {"isDefault", true}
        }}
      };

      json RebuildTaskJson = {
        {"type", "shell"},
        {"label", "Rebuild Engine"},
        {"command", RyBuildPathWorkspaceRelative},
        {"args", json::array({
          "rebuild",
          RyBuildModulesPathWorkspaceRelative
        })},
        {"options", {
          {"cwd", RyToolsBinaryWorkspaceRelative}
        }},
        {"problemMatcher", json::array({"$gcc"})},
        {"group", {
          {"kind", "build"},
          {"isDefault", false}
        }}
      };

      json CleanTaskJson = {
        {"type", "shell"},
        {"label", "Clean Engine"},
        {"command", RyBuildPathWorkspaceRelative},
        {"args", json::array({
          "clean",
          RyBuildModulesPathWorkspaceRelative
        })},
        {"options", {
          {"cwd", RyToolsBinaryWorkspaceRelative}
        }},
        {"problemMatcher", json::array({"$gcc"})},
        {"group", {
          {"kind", "build"},
          {"isDefault", true}
        }}
      };

      RootTasksJson["tasks"] = json::array({BuildTaskJson, RebuildTaskJson, CleanTaskJson});

      std::ofstream Out(TasksJsonPath);
      Out << RootTasksJson.dump(4); // Pretty indentations
      Out.close(); 
    }

    // Generate c_cpp_properties.json
    {
      Filesystem::path CppPropertiesPath  = VsCodePath / "c_cpp_properties.json";

      json IncludePathArray = json::array();
      for(auto& IncludePath : IncludePaths)
        IncludePathArray.push_back("${workspaceFolder}/" + PathRelativeTo(GetEngineRootDir(), IncludePath).string());

      json ConfigJson = {
        {"name", "Default"},
        {"includePath", IncludePathArray},
        {"compilerPath", "gcc"},
        {"cStandard", "gnu17"},
        {"cppStandard", "gnu++17"},
        {"intelliSenseMode", "macos-clang-arm64"}
      };

      json RootPropsJson = {
        {"configurations", json::array({ConfigJson})},
        {"version", 4}
      };

      std::ofstream Out(CppPropertiesPath);
      Out << RootPropsJson.dump(4); // Pretty indentations
      Out.close(); 
    }

    // Generate launch.json
    {
      json RootLaunchJson;
      json ConfigsJson;
      Filesystem::path LaunchPath  = VsCodePath / "launch.json";

      if(Filesystem::exists(LaunchPath))
      {
        RootLaunchJson = json::parse(ReadFileAsString(LaunchPath.string()));
      }

      std::vector<std::string> LaunchArgs;

      if(GetHostOS() == OSType::OSX)
      {
        LaunchArgs.push_back("-OpenGL");
      }

      std::string EditorBinary = GetEditorExecutable();
      std::string EditorRelative = "${workspaceFolder}/" + PathRelativeTo(GetEngineRootDir(), EditorBinary).string();
      std::string WorkingDir = "${workspaceFolder}/" + PathRelativeTo(GetEngineRootDir(), GetEngineBinaryDir()).string();

      json ArgsJson = json::array();
      for(auto& Arg : LaunchArgs)
        ArgsJson.push_back(Arg);

      int MatchingConfig = -1;
      int CurConfig = 0;
      if(RootLaunchJson.contains("configurations"))
      {
        ConfigsJson = RootLaunchJson["configurations"];
        for(auto& Config : ConfigsJson)
        {
          if(Config.contains("name") && Config["name"] == "Launch Editor")
          {
            MatchingConfig = CurConfig;
            break;
          }
          CurConfig++;   
        }

      }

      std::string LaunchType;
		if(Compiler == "MSVC")
		{
            LaunchType = "cppvsdbg";
		}
        else
        {
            LaunchType = "lldb";
        }

      json LaunchJson = {
        {"type", LaunchType},
        {"request", "launch"},
        {"name", "Launch Editor"},
        {"program", EditorRelative},
        {"args", ArgsJson},
        {"cwd", WorkingDir}
      };

      if(MatchingConfig >= 0)
        ConfigsJson[MatchingConfig] = LaunchJson;      
      else if(ConfigsJson.size() <= 0)
        ConfigsJson = json::array({LaunchJson});
      else
        ConfigsJson.push_back(LaunchJson);

      RootLaunchJson = {
        {"configurations", ConfigsJson},
        {"version", "0.2.0"}
      };

      std::ofstream Out(LaunchPath);
      Out << RootLaunchJson.dump(4); // Pretty indentations
      Out.close(); 
    }


}

bool GenerateGameProjectFiles_VSCode(std::string GameName, std::string GameRootPath, std::string EngineRootPath, std::string Compiler)
{
    return false;
}