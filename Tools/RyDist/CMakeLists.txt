# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Set the sources for the project
set (SOURCES
	Source/Implementation/Entry.cpp
)

include_directories(Source/Include ../RyCommon/Source/Include)

add_executable(RyDist ${SOURCES})

set_target_properties(RyDist PROPERTIES
            CXX_STANDARD 17
			CMAKE_CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)

target_link_libraries(RyDist PUBLIC RyCommon)

if(WIN32)
	target_link_libraries(RyDist PRIVATE rpcrt4)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyDist PRIVATE -lstdc++fs)
endif()