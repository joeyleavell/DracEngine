cmake_minimum_required (VERSION 3.8)

set (SOURCES
	Source/Implementation/Reflection.cpp
)

# External includes
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
include_directories(Source/Include ../RyCommon/Source/Include)

# Todo: Determine best way to switch architectures, probably using a flag
if(MSVC)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/x64/Windows/MSVC")
elseif(MinGW)
	# Todo: MinGW libs
elseif(UNIX AND NOT APPLE)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/x64/Linux/GCC")
elseif(APPLE)
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/Platform/Libraries/ARM64/OSX/GCC")
endif()

# Platform includes
if(WIN32)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Windows)
elseif(UNIX AND NOT APPLE)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/Linux)
elseif(APPLE)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Platform/Include/OSX)
endif()

add_executable(RyReflect ${SOURCES})

set_target_properties(RyReflect PROPERTIES
            CXX_STANDARD 17
			CMAKE_CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)

# Link with RyCommon
target_link_libraries(RyReflect PUBLIC RyCommon)

set_property(TARGET RyReflect PROPERTY CXX_STANDARD 17)

# Have to use separate threads library for MinGW builds
if(MinGW)
	message("${PROJECT_NAME}: mingw ${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../mingw-std-threads/cmake_stdheaders_generator/cmake_stdheaders_generator)
endif()

# Windows specific links
if(MinGW OR WIN32)
	target_link_libraries(RyReflect PRIVATE rpcrt4)
	target_link_libraries(RyReflect PRIVATE version) # Used in llvm
endif()

target_link_libraries(RyReflect PRIVATE clangDynamicASTMatchers)
target_link_libraries(RyReflect PRIVATE clangFormat)
target_link_libraries(RyReflect PRIVATE clangDriver)

target_link_libraries(RyReflect PRIVATE clangAST)
target_link_libraries(RyReflect PRIVATE clangASTMatchers)
target_link_libraries(RyReflect PRIVATE clangBasic)
target_link_libraries(RyReflect PRIVATE clangFrontend)
target_link_libraries(RyReflect PRIVATE clangSerialization)
target_link_libraries(RyReflect PRIVATE clangToolingInclusions)
target_link_libraries(RyReflect PRIVATE clangToolingASTDiff)
target_link_libraries(RyReflect PRIVATE clangToolingRefactoring)
target_link_libraries(RyReflect PRIVATE clangToolingSyntax)
target_link_libraries(RyReflect PRIVATE clangToolingCore)
target_link_libraries(RyReflect PRIVATE clangTooling)
target_link_libraries(RyReflect PRIVATE clangTesting)
target_link_libraries(RyReflect PRIVATE clangFrontendTool)
target_link_libraries(RyReflect PRIVATE clangTransformer)
target_link_libraries(RyReflect PRIVATE clangInterpreter)

target_link_libraries(RyReflect PRIVATE clangParse)
target_link_libraries(RyReflect PRIVATE LLVMMCParser)
target_link_libraries(RyReflect PRIVATE clangSema)
target_link_libraries(RyReflect PRIVATE clangEdit)
target_link_libraries(RyReflect PRIVATE clangAnalysis)
target_link_libraries(RyReflect PRIVATE LLVMBitReader)
target_link_libraries(RyReflect PRIVATE LLVMProfileData)
target_link_libraries(RyReflect PRIVATE clangRewrite)
target_link_libraries(RyReflect PRIVATE clangRewriteFrontend)
target_link_libraries(RyReflect PRIVATE clangLex)
target_link_libraries(RyReflect PRIVATE LLVMCore)
target_link_libraries(RyReflect PRIVATE LLVMBinaryFormat)
target_link_libraries(RyReflect PRIVATE LLVMMC)
target_link_libraries(RyReflect PRIVATE LLVMOption)
target_link_libraries(RyReflect PRIVATE LLVMSupport)
target_link_libraries(RyReflect PRIVATE z)
target_link_libraries(RyReflect PRIVATE curses)
target_link_libraries(RyReflect PRIVATE LLVMDemangle)

target_link_libraries(RyReflect PRIVATE clangHandleCXX)
target_link_libraries(RyReflect PRIVATE clangHandleLLVM)
target_link_libraries(RyReflect PRIVATE clangIndex)
target_link_libraries(RyReflect PRIVATE LLVMTarget)
target_link_libraries(RyReflect PRIVATE LLVMDWARFLinker)
target_link_libraries(RyReflect PRIVATE LLVMRemarks)
target_link_libraries(RyReflect PRIVATE LLVMFrontendOpenMP)
target_link_libraries(RyReflect PRIVATE LLVMBitstreamReader)
target_link_libraries(RyReflect PRIVATE LLVMObject)
target_link_libraries(RyReflect PRIVATE LLVMMCA)
target_link_libraries(RyReflect PRIVATE LLVMSparcInfo)

target_compile_options(RyReflect PUBLIC -fno-rtti)

# Linux specific libraries
if(UNIX AND NOT APPLE)
	target_link_libraries(RyReflect PRIVATE dl)
	target_link_libraries(RyReflect PRIVATE util)
endif()

# GCC specific libraries
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(RyReflect PRIVATE -lstdc++fs)
	target_link_libraries(RyReflect PRIVATE -lpthread)
endif()

install(TARGETS RyReflect DESTINATION .)