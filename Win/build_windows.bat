
@echo off
setlocal

set dir_name=build
if exist %dir_name% (
    echo delete %dir_name%
    rd /s/q %dir_name%
)
md %dir_name%
echo create %dir_name%
cd %dir_name%

cmake ..

cmake --build .