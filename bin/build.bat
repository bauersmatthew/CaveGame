@echo off

IF NOT EXIST w:\build mkdir w:\build
pushd w:\build

del *.pdb > NUL 2> NUL

set Game_DebugCompilerFlags=-Od -MTd -nologo -Gr -EHa -Oi -W4 -Z7 -FC -Fegame.dll -LD -wd4100
set Game_ReleaseCompilerFlags=-O2 -MTd -nologo -Gr -EHa -W4 -FC -Fegame.dll -LD -wd4100
set Game_LinkerFlags=/NODEFAULTLIB:MSVCRT /SUBSYSTEM:windows opengl32.lib -PDB:game_%random%.pdb -EXPORT:start_up -EXPORT:main_loop -EXPORT:shut_down -EXPORT:on_reload -EXPORT:on_unload

set Platform_DebugCompilerFlags=-Od -MTd -nologo -Gr -EHa -Oi -W4 -Z7 -FC -Femain.exe -wd4100
set Platform_ReleaseCompilerFlags=-O2 -MTd -nologo -Gr -EHa -W4 -FC -Femain.exe -wd4100
set Platform_LinkerFlags=/NODEFAULTLIB:MSVCRT /SUBSYSTEM:console kernel32.lib user32.lib gdi32.lib opengl32.lib 

if "%1"=="release" (
	cl %Game_ReleaseCompilerFlags% w:\src\game.cpp /link %Game_LinkerFlags%
) else if "%2"=="release" (
	cl %Game_ReleaseCompilerFlags% w:\src\game.cpp /link %Game_LinkerFlags%
) else (
	cl %Game_DebugCompilerFlags% w:\src\game.cpp /link %Game_LinkerFlags%
)

if "%1"=="platform" goto platform
goto :eof

:platform
if "%2"=="release" (
	cl %Platform_ReleaseCompilerFlags% w:\src\platform_main.cpp /link %Platform_LinkerFlags%
) else (
	cl %Platform_DebugCompilerFlags% w:\src\platform_main.cpp /link %Platform_LinkerFlags%
)

popd