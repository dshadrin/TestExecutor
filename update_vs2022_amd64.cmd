@echo off
cd .build-debug

cmake .. -G "Visual Studio 17 2022" -A x64 -Wno-dev

pause
