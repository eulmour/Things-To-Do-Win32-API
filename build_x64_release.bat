@echo off & setlocal enabledelayedexpansion
set dir_binary=x64\Release
set app_name=ThingsToDo
set src_ext=.c
set app_std=/std:c17

set dir_build=build
set dir_project=ThingsToDo
set dir_src=%dir_project%\src

::project directories
set dir_paths=res winapi

::include paths
for %%a in (%dir_paths%) do (
    set cur=%%a
    set include_paths=!include_paths! /I %dir_project%\!cur!
)

::compiler flags
set compile_flags=/W3 /WX- /D NDEBUG /D _WINDOWS /EHsc /MD /O2 /Oi /GL /Gm- /Gs /Gy /fp:precise %app_std% /Fd:%dir_build%\\vc140.pdb
set link_flags=/SUBSYSTEM:WINDOWS /MACHINE:X64 /DYNAMICBASE /ERRORREPORT:PROMPT /NOLOGO
set lib_files=Comctl32.lib Shlwapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
set manifest=/MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /manifestinput:res\ThingsToDo.exe.manifest

set files=/Fo.\%dir_build%\ /c

for /r %cd%\%dir_src% %%f in (*.c) do (
    set cur=%%f
    set src_cur=!cur:%cd%\=!
    set obj_cur=!src_cur:%dir_src%=!
    set obj_cur=%dir_build%!obj_cur:%src_ext%=.obj!

    set object_files=!object_files! !obj_cur!
    set files=!files! !src_cur!
)

echo %files%
@REM echo %object_files%

@REM if not exist %dir_build% mkdir %dir_build%
@REM if not exist %dir_build%\winapi mkdir %dir_build%\winapi

@REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
@REM cl /nologo /Zi %compile_flags% %include_paths% %files%
@REM link /OUT:%dir_binary%\%app_name%.exe %link_flags% %lib_files% %manifest% %object_files:~1%