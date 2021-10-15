# Fully example commands for debuggin
set -x

echo $(dirname "$0")

# Get cpython version 3.11.0a1
git clone https://github.com/python/cpython.git
cd cpython
git checkout tags/v3.11.0a1

# Configure python to output a universal binary for mac so we don't have to maintain multiple architectures for osx
# Universal2 specifies x86_64 and arm64
# Use the GCC version 11 C++ compiler
# Install the files in the same directory
./configure --enable-universalsdk -with-universal-archs=universal2 --with-cxx-main=g++-11 --enable-optimizations --prefix=$1

# Build python using 8 jobs
make -j 8

# Install python
make install