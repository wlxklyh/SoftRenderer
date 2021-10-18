@echo off
setlocal

REM 生成VS工程的

REM （1）检查cmake是否存在
where cmake
if %errorlevel%==0 (
    echo cmake found
) else (
    echo cmake not found
    goto exit_build
)


REM （2）检查build文件夹是否存在 存在要重新生成
set dir_name=build
if exist %dir_name% (
    echo delete %dir_name%
    rd /s/q %dir_name%
)
md %dir_name%
echo create %dir_name%
cd %dir_name%


REM （3）cmake
cmake -G "Visual Studio 16 2019" ..

if %errorlevel%==0 (
    echo.
    echo.
    echo solution is generated in the %dir_name%
    echo.
    echo.
) else (
    echo.
    echo.
    echo solution generating is failed!!! Please fix the problem before cmake.
    echo.
    echo.

    goto exit_build
)