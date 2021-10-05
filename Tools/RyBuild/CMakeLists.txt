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

# External includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include/Python)
include_directories(Source/Include ../RyCommon/Source/Include)

# Todo: Determine best way to switch architectures, probably using a flag
if(MSVC)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/x64/Windows/MSVC")
elseif(MinGW)
	# Todo: MinGW libs
elseif(UNIX AND NOT APPLE)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/x64/Linux/GCC")
elseif(APPLE)
	# Todo: Apple libs
endif()

# Platform includes
if(WIN32)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Windows)
elseif(UNIX AND NOT APPLE)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Linux)
elseif(APPLE)
	# Todo: apple includes
endif()

add_executable(RyBuild ${SOURCES})

set_target_properties(RyBuild PROPERTIES
            CXX_STANDARD 17
	    CMAKE_CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)

# Link with RyCommon
target_link_libraries(RyBuild PUBLIC RyCommon)
#target_link_libraries(RyBuild PUBLIC python310)

set_property(TARGET RyBuild PROPERTY CXX_STANDARD 17)

# Have to use separate threads library for MinGW builds
if(MinGW)
	message("${PROJECT_NAME}: mingw ${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator)
endif()

# Windows specific links
if(MinGW OR WIN32)
	target_link_libraries(RyBuild PRIVATE rpcrt4)
	target_link_libraries(RyBuild PRIVATE python310)
endif()

# Linux specific libraries
if(UNIX AND NOT APPLE)
	target_link_libraries(RyBuild PRIVATE python3.10)
	target_link_libraries(RyBuild PRIVATE dl)
	target_link_libraries(RyBuild PRIVATE util)
endif()

# GCC specific libraries
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyBuild PRIVATE -lstdc++fs)
	target_link_libraries(RyBuild PRIVATE -lpthread)
endif()

install(TARGETS RyBuild DESTINATION .)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Python/Lib/ DESTINATION ${CMAKE_INSTALL_PREFIX}/PythonLib)

if(MSVC)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Binary/x64/Windows/MSVC/python310.dll DESTINATION ${CMAKE_INSTALL_PREFIX})
elseif(MinGW)
	# Todo: MinGW dlls
elseif(UNIX AND NOT APPLE)
	# Todo: Linux SOs
elseif(APPLE)
	# Todo: Apple SOs
endif()
