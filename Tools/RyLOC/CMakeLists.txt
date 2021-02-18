include_directories(Source/Include)

set (SOURCES
	Source/Implementation/Entry.cpp
)

add_executable(RyLOC ${SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyLOC PRIVATE -lstdc++fs)
endif()