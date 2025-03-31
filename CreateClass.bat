@echo off
setlocal

:: Get the directory where the batch file is located
set "base_dir=%~dp0"

:: Prompt for the class name
set /p classname=Enter the class name:

:: Prompt for the subdirectory name
set /p subdirname=Enter the subdirectory name:

:: Define the base src directory (assumed to be inside the batch file directory)
set "src_dir=%base_dir%src"

:: Define public and private directories with the subdirectory name inside them
set "public_dir=%src_dir%\public\%subdirname%"
set "private_dir=%src_dir%\private\%subdirname%"

:: Create directories if they don't exist
if not exist "%public_dir%" (
    mkdir "%public_dir%"
    echo Created public directory: %public_dir%
)
if not exist "%private_dir%" (
    mkdir "%private_dir%"
    echo Created private directory: %private_dir%
)

:: Define file paths
set "header_file=%public_dir%\%classname%.hpp"
set "source_file=%private_dir%\%classname%.cpp"

:: Create empty header and source files
type nul > "%header_file%"
type nul > "%source_file%"

echo Files created successfully:
echo Header: %header_file%
echo Source: %source_file%

build-VisualStudio2022.bat

endlocal
pause
