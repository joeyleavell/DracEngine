cmake_minimum_required (VERSION 3.8)

set (SOURCES
	Source/Implementation/Entry.cpp
	Source/Implementation/Build/Module/Module.cpp
	Source/Implementation/Build/BuildTool.cpp
	Source/Implementation/Build/MSVCBuildTool.cpp
	Source/Implementation/Build/GCCBuildTool.cpp
	Source/Implementation/Generate/Windows/WindowsProject.cpp
	Source/Implementation/Generate/Windows/WindowsProjectBase.cpp
	Source/Implementation/Generate/Windows/WindowsProjectFolder.cpp
	Source/Implementation/Generate/Windows/WindowsProjectSolution.cpp
	Source/Implementation/Generate/Project.cpp
)

include_directories(Source/Include ../RyCommon/Source/Include)

add_executable(RyBuild ${SOURCES})

set_target_properties(RyBuild PROPERTIES
            CXX_STANDARD 17
			CMAKE_CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)

target_link_libraries(RyBuild PUBLIC RyCommon)

if(MinGW)
	message("${PROJECT_NAME}: mingw ${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator)
endif()

if(MinGW OR WIN32)
	target_link_libraries(RyBuild PRIVATE rpcrt4)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyBuild PRIVATE -lstdc++fs)
	target_link_libraries(RyBuild PRIVATE -lpthread)
endif()

install(TARGETS RyBuild DESTINATION .)