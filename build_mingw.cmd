@echo off
rem set PATH=c:\msys64\mingw64\bin;%PATH%
rmdir /S /Q build
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -Wno-dev

pause
