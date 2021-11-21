@echo off & setlocal enabledelayedexpansion
set dir_binary=bin\Debug
set app_name=dynstring
set src_ext=.cpp

set dir_build=build
set dir_src=src

set compile_flags=/W3 /D _DEBUG /D _CONSOLE /D _UNICODE /D UNICODE /EHsc /MDd /Fd:%dir_build%\\vc140.pdb
set link_flags=/DEBUG /SUBSYSTEM:CONSOLE /MACHINE:X64 /DYNAMICBASE /TLBID:1 /NXCOMPAT /ERRORREPORT:PROMPT /NOLOGO

set files=/Fo.\%dir_build%\ /c

for /r %cd%\%dir_src% %%f in (*.cpp) do (
    set cur=%%f
    set src_cur=!cur:%cd%\=!
    set obj_cur=!src_cur:%dir_src%=!
    set obj_cur=%dir_build%!obj_cur:%src_ext%=.obj!

    set object_files=!object_files! !obj_cur!
    set files=!files! !src_cur!
)

if not exist %dir_binary%\i386 mkdir %dir_binary%\i386
if not exist %dir_binary%\x86_64 mkdir %dir_binary%\x86_64

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
cl /nologo /Zi %compile_flags% %files%
link /OUT:%dir_binary%\x86_64\%app_name%.exe %link_flags% %object_files:~1%