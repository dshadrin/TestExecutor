#!/bin/sh
rm -fr build
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Debug" -DUNIX=ON
pause
cmake --build ./ -j 3 --target install

