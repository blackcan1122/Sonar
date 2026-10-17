@echo off
cmake --preset msvc || exit /b 1
cmake --build --preset msvc-debug || exit /b 1
