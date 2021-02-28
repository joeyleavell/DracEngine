# CMake build the engine rools
cmake -DCMAKE_INSTALL_PREFIX="./Tools/Binary" -H"./Tools" -B"./Tools/Build/GCC"
cmake --build "./Tools/Build/GCC" --config Release
cmake --build "./Tools/Build/GCC" --target install --config Release
