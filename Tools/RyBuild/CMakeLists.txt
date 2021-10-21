cmake_minimum_required (VERSION 3.8)

set (SOURCES
	Source/Implementation/Entry.cpp
	Source/Implementation/Build/Module/Module.cpp
	Source/Implementation/Build/BuildTool.cpp
	Source/Implementation/Build/MSVCBuildTool.cpp
	Source/Implementation/Build/GCCBuildTool.cpp
	Source/Implementation/Deps/BuildDeps.cpp
	Source/Implementation/Generate/Windows/WindowsProject.cpp
	Source/Implementation/Generate/Windows/WindowsProjectBase.cpp
	Source/Implementation/Generate/Windows/WindowsProjectFolder.cpp
	Source/Implementation/Generate/Windows/WindowsProjectSolution.cpp
	Source/Implementation/Generate/Project.cpp
)

############### Platform Libraries ############################
if(MSVC)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/x64/Windows/MSVC")
elseif(MinGW)
	# Todo: MinGW libs
elseif(UNIX AND NOT APPLE)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/x64/Linux/GCC")
elseif(APPLE)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/ARM64/OSX/GCC")
endif()
###############################################################

add_executable(RyBuild ${SOURCES} "Source/Implementation/Generate/ProjectFileGenerator.cpp" "Source/Implementation/Generate/GenerateModule.cpp" "Source/Include/Generate/GenerateGame.h" "Source/Implementation/Generate/GenerateGame.cpp" "Source/Implementation/Generate/Windows/VisualStudioProjectFileGenerator.cpp")
include_directories(Source/Include)

################ Platform Includes ############################
if(WIN32)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Windows)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Windows/Python)
elseif(UNIX AND NOT APPLE)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Linux)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Linux/Python)
elseif(APPLE)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/OSX)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/OSX/Python)
endif()
###############################################################

################# Target Properties ##########################
set_target_properties(RyBuild PROPERTIES
            CXX_STANDARD 17
			CMAKE_CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)
##############################################################

################# COMMON #####################################
target_link_libraries(RyBuild PUBLIC RyCommon)
include_directories(../RyCommon/Source/Include)
##############################################################

################# Windows PThreads (MinGW) ###################
if(MinGW)
	message("${PROJECT_NAME}: mingw ${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator)
endif()
##############################################################

################### LINK PYTHON ###################
if(UNIX)
	target_link_libraries(RyBuild PRIVATE python3.11)
elseif(WIN32)
	target_link_libraries(RyBuild PRIVATE python311)
endif()
###################################################

################### System Libs ###################
if(MinGW OR WIN32)
	target_link_libraries(RyBuild PRIVATE rpcrt4)
elseif(UNIX AND NOT APPLE)
	# Linux specific libraries
	target_link_libraries(RyBuild PRIVATE dl)
	target_link_libraries(RyBuild PRIVATE util)
elseif(APPLE)
	# Apple specific libraries
endif()
###################################################

################## Compiler Libs ##################
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyBuild PRIVATE -lstdc++fs)
	target_link_libraries(RyBuild PRIVATE -lpthread)
endif()
###################################################

########################## INSTALL ################################
install(TARGETS RyBuild DESTINATION .)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Python/Lib/ DESTINATION ./PythonLib)

# Python on windows is a DLL
if(MSVC)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Binary/x64/Windows/MSVC/python311.dll DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
####################################################################