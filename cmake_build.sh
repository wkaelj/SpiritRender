#!/bin/bash
build_dir=build

# cmake -B $build_dir/  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build $build_dir/ 

./build/SpiritRender @a
