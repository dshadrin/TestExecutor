#!/bin/sh
rm -fr build
mkdir build
cd build
/home/dima/opt/Qt/Tools/CMake/bin/cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING="Debug" -DUNIX=ON
pause
/home/dima/opt/Qt/Tools/CMake/bin/cmake --build ./ -j 3 --target install

