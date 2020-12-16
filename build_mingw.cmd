@echo off
rem set PATH=d:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin;%PATH%
rmdir /S /Q build
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -Wno-dev

pause
