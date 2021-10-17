include_directories(Source/Include)

set (SOURCES
	Source/Implementation/Entry.cpp
)

add_executable(RyLOC ${SOURCES})

# Add RyCommon includes and link with it
include_directories(Source/Include ../RyCommon/Source/Include)
target_link_libraries(RyLOC PUBLIC RyCommon)

set_target_properties(RyLOC PROPERTIES
            CXX_STANDARD 17
			CMAKE_CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyLOC PRIVATE -lstdc++fs)
endif()

install(TARGETS RyLOC DESTINATION .)