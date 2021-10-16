:: Get llvm version 14
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git checkout tags/llvmorg-14-init

:: Generate project files for 64 bit windows (we don't support 32 bit builds)
cmake -Hllvm -BBuild -DLLVM_ENABLE_PROJECTS='clang' -DCMAKE_BUILD_TYPE=Release -Thost=x64

:: Build llvm
cmake --build Build --config Release

:: Install llvm
cmake --install Build --config Release --prefix Install

:: TODO: Copy over libraries we need