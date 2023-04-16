@echo off
cd /d %~dp0
set PATH=%cd%\map;%PATH%
echo %PATH%
.\penguins_server
