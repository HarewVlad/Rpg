@echo off

REM EHsc is used instead of EHa for now to satisfy warning
REM Disabled C4244 because i don't like to convert to float ...
set CommonCompilerFlags=-diagnostics:column -WL -nologo -fp:fast -fp:except- -Gm- -GR- -EHsc- -Zo -Oi -WX -W4 -wd4244 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999
set CommonLinkerFlags=-STACK:0x100000,0x100000 -incremental:no -opt:ref

cl %CommonCompilerFlags% -MTd main.cpp /Iinclude /link /SUBSYSTEM:windows %CommonLinkerFlags%