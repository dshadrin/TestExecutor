@echo off
rmdir /S /Q .build-debug
mkdir .build-debug
cd .build-debug
cmake .. -G "Visual Studio 17 2022" -A x64 -Wno-dev -DMSVC=ON 

pause
