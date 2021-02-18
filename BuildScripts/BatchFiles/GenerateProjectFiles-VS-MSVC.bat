echo off

setlocal EnableDelayedExpansion

rem Module root relative to binaries directory
set SOLUTIONROOT="../../../"
set BINARYDIR="../../Tools/RyBuild/Binary/"

rem Perform work in engine binary directory
pushd %~dp0

	rem Change to binaries directory
	cd %BINARYDIR%
	
	rem Invoke build tool on the root module path with the found cl.exe
	call RyBuild.exe generate ProjectFiles -Engine -MSVC "%SOLUTIONROOT%"

popd