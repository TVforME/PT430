@echo off
setlocal enabledelayedexpansion

:: Check if version.txt exists
if not exist version.txt (
    echo Error: version.txt not found!
    exit /b 1
)

:: Read version from version.txt
for /f "tokens=*" %%a in (version.txt) do set VERSION_STRING=%%a

:: Parse version components
for /f "tokens=1,2,3 delims=." %%a in ("%VERSION_STRING%") do (
    set VERSION_MAJOR=%%a
    set VERSION_MINOR=%%b
    set VERSION_PATCH=%%c
)

:: Remove existing version.h if it exists
if exist version.h (
    del version.h
    echo Removed existing version.h
)

:: Generate version.h
echo Generating version.h...
(
echo #ifndef VERSION_H
echo #define VERSION_H
echo.
echo #define VERSION_MAJOR    %VERSION_MAJOR%
echo #define VERSION_MINOR    %VERSION_MINOR%
echo #define VERSION_PATCH    %VERSION_PATCH%
echo #define VERSION_STRING   "%VERSION_STRING%"
echo #define BUILD_DATE      "%DATE%"
echo #define BUILD_TIME      "%TIME%"
echo #define BUILD_YEAR      "%DATE:~-4%"
echo.
echo #endif // VERSION_H
) > version.h
echo New version.h generated

:: Build
echo Building on Windows...
mingw32-make clean
mingw32-make

:: Check build status
if errorlevel 1 (
    echo Build failed!
    exit /b 1
) else (
    echo Build successful!
    echo Version: %VERSION_STRING% (%DATE% %TIME%)
    echo Copyright (c) %DATE:~-4%
)

endlocal
