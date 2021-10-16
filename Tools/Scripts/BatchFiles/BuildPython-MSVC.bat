:: Get python version 3.11
git clone https://github.com/python/cpython.git
cd cpython
git checkout tags/v3.11.0a1

cd PCBuild
call build.bat

cd ../..