echo off
set IncludeRT=C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\cppwinrt
set libs=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64
if not exist out\ ( md out) 



call windres stuff.rc -o out/res.o
call gcc ini.c gui.c out/res.o -mwindows -DDEBUG -lcomctl32 -o out/Zeus(debug).exe  

