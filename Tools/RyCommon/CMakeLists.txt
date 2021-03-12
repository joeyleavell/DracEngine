# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Set the sources for the project
set (SOURCES
	Source/Implementation/Util/Util.cpp
)

include_directories(Source/Include)

add_library(RyCommon STATIC ${SOURCES} "Source/Implementation/Common.cpp")

set_property(TARGET RyCommon PROPERTY CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyCommon PRIVATE -lstdc++fs)
endif()
