@echo off
cmake --preset mingw-debug || exit /b 1
cmake --build --preset mingw-debug || exit /b 1
