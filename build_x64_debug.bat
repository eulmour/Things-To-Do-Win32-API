@echo off & setlocal enabledelayedexpansion
::projejct meta
set dir_binary=x64\Debug
set app_name=ThingsToDo
set src_ext=.c
set app_std=/std:c17

set dir_build=build
set dir_project=ThingsToDo
set dir_src=%dir_project%\src

::project directories
set DIR_PATHS=res src\winapi

::include paths
for %%a in (%DIR_PATHS%) do (
    set cur=%%a
    set INCLUDE_PATHS=!INCLUDE_PATHS! /I %dir_project%\!cur!
)

::flags
set COMPILER_FLAGS=^
/D _DEBUG ^
/D _WINDOWS ^
/D _UNICODE ^
/D UNICODE ^
/nologo ^
/Zi ^
/JMC ^
/W3 ^
/WX- ^
/Gm- ^
/Gs ^
/Gd ^
/TC ^
/FC ^
/EHsc ^
/MDd ^
%app_std% /Fd:%dir_build%\vc140.pdb

set LINK_FLAGS=^
/DEBUG ^
/SUBSYSTEM:WINDOWS ^
/MACHINE:X64 ^
/DYNAMICBASE ^
/TLBID:1 ^
/NXCOMPAT ^
/ERRORREPORT:PROMPT ^
/NOLOGO

set LIB_FILES=^
Comctl32.lib ^
Shlwapi.lib ^
kernel32.lib ^
user32.lib ^
gdi32.lib ^
winspool.lib ^
comdlg32.lib ^
advapi32.lib ^
shell32.lib ^
ole32.lib ^
oleaut32.lib ^
uuid.lib ^
odbc32.lib ^
odbccp32.lib

set MANIFEST=^
/MANIFEST ^
/MANIFESTUAC:"level='asInvoker' uiAccess='false'" ^
/manifest:embed ^
/manifestinput:%dir_project%\res\ThingsToDo.exe.manifest

::source files
set SOURCE_FILES=/Fo.\%dir_build%\ /c
for /r %cd%\%dir_src% %%f in (*.c) do (
    set cur=%%f
    set src_cur=!cur:%cd%\=!
    set obj_cur=%%~nxf
    set obj_cur=%dir_build%\!obj_cur:%src_ext%=.obj!

    set OBJECT_FILES=!OBJECT_FILES! !obj_cur!
    set SOURCE_FILES=!SOURCE_FILES! !src_cur!
)

::resources
for /r %cd%\%dir_project%\res %%f in (*.rc) do (
    set cur=%%f
    set rc_cur=!cur:%cd%\=!
    set rc_file_cur=%%~nxf
    set rc_file_cur=%dir_build%\!rc_file_cur:.rc=.res!
    set RES_FILES=!RES_FILES! !rc_file_cur!
    set RESOURCES=!RESOURCES! /fo.\!rc_file_cur! !rc_cur!
)

::create build directory
if not exist %dir_build% mkdir %dir_build%

::build
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cl %COMPILER_FLAGS% %INCLUDE_PATHS% %SOURCE_FILES%
rc /D _UNICODE /D UNICODE /l"0x0409" %INCLUDE_PATHS% /nologo %RESOURCES%
link /OUT:%dir_binary%\%app_name%.exe %LINK_FLAGS% %LIB_FILES% %MANIFEST% %OBJECT_FILES:~1% %RES_FILES:~1%