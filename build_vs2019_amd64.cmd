@echo off
rmdir /S /Q .build-msvc
mkdir .build-msvc
cd .build-msvc
cmake .. -G "Visual Studio 16 2019" -A x64 -Wno-dev

pause
