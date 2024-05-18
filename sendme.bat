@echo off

REM yours may vary
set PATH=%PATH%;d:\notpsxserial\

REM yours may vary
nops /ftdi /exe mcprotest.exe COM5 /m
