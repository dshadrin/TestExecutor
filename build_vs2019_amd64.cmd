@echo off
rmdir /S /Q build
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -Wno-dev

pause
