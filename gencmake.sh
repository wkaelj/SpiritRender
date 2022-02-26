#! /bin/bash

#create build folder
mkdir -p build/Config/shaders
#copy config files into build folder
cp Config/*.txt build/Config/*.txt

./compile_shaders.sh
#run cmake
cmake -S . -B build/