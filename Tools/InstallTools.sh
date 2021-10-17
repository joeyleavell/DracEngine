# Set your compiler here
export RY_CXX_COMPILER=g++
export RY_C_COMPILER=gcc

# CMake build the engine rools
cmake -H. -B"./Build" -DCMAKE_CXX_COMPILER=$RY_CXX_COMPILER -DCMAKE_C_COMPILER=$RY_C_COMPILER

cmake --build "./Build" --config Debug

cmake --install "./Build" --config Debug --prefix=./Binary
