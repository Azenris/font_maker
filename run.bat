@echo OFF
cls
SET mypath=%~dp0
pushd %mypath%\TEMP\
start font_maker.exe
popd