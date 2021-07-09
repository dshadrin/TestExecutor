@echo off
cd .build-debug

cmake .. -G "Visual Studio 16 2019" -A x64 -Wno-dev

pause
