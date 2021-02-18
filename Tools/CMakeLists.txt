cmake_minimum_required (VERSION 3.8)

# Avoid this check since it can fail when cross compiling
#set (CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set where the produced executables go
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Binary)

project ("RyTools")

# Add the mingw project to the project tree if we want to compile with mingw
add_subdirectory(RyCommon)
add_subdirectory(RyBuild)
add_subdirectory(RyDist)
add_subdirectory(RyLOC)