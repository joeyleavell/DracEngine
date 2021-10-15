# Set your compiler here
export RY_CXX_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0/bin/g++-11
export RY_C_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0/bin/gcc-11

# CMake build the engine rools
cmake -DCMAKE_INSTALL_PREFIX=./Binary -H. -B"./Build" -DCMAKE_CXX_COMPILER=$RY_CXX_COMPILER -DCMAKE_C_COMPILER=$RY_C_COMPILER

cmake --build "./Build" --config Release

cmake --build "./Build" --target install --config Release
