@echo off
rem set PATH=c:\msys64\mingw64\bin;%PATH%
rmdir /S /Q .build-mingw
mkdir .build-mingw
cd .build-mingw
cmake .. -G "MinGW Makefiles" -Wno-dev

pause
