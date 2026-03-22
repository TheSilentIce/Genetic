@echo off
rd /s /q build
cmake -B build
cmake --build build
start "" cli.exe
