@ Run CMake build the engine tools
cmake -S ./Tools -B ./Tools/Build
cmake --build ./Tools/Build --config Release
cmake --install ./Tools/Build --prefix ./Tools/Binary --config Release
rmdir /Q /S Tools\Build