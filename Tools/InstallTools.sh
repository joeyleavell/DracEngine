# Set your compiler here
export RY_CXX_COMPILER=g++-11
export RY_C_COMPILER=gcc-11

# CMake build the engine rools
cmake -H. -B"./Build" -DCMAKE_CXX_COMPILER=$RY_CXX_COMPILER -DCMAKE_C_COMPILER=$RY_C_COMPILER

cmake --build "./Build" --config RelWithDebInfo

cmake --install "./Build" --config RelWithDebInfo --prefix=./Binary
