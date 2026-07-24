#!/bin/bash 

cd ./build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DTESTS=False -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
