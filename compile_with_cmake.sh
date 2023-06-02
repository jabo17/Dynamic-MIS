#!/bin/bash

# args
if [ "$#" -ne 1 ]; then
    echo "Illegal number of arguments. Use ./compile_withcmake <buildtype>, e.g. /compile_withcmake Release."
    exit
fi
# build-type options: Release,Debug
declare buildtype=$1

# determine CPUs
declare NCORES=4
declare unamestr=$(uname)
if [[ "$unamestr" == "Linux" ]]; then
    NCORES=$(grep -c ^processor /proc/cpuinfo)
fi

if [[ "$unamestr" == "Darwin" ]]; then
    NCORES=$(sysctl -n hw.ncpu)
fi

# build
rm -rf build
mkdir build
cd build
if [[ "$unamestr" == "Linux" ]]; then
    cmake ../ -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE=${buildtype}
fi
if [[ "$unamestr" == "Darwin" ]]; then
    cmake ../ -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE=${buildtype} -DCMAKE_C_COMPILER=/usr/local/bin/gcc-12 -DCMAKE_CXX_COMPILER=/usr/local/bin/g++-12
fi
make -j $NCORES

# test
ctest

# deploy
cmake --install .
cd ..

echo "Build completed."
