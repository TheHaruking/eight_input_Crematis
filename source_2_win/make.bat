@echo off
set VCVARSALL_PATH="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
set BUILD_COMMAND=""

IF {%1} == {set} (
  goto SETVAR
) ELSE (
  goto BUILD
)

:SETVAR
  %VCVARSALL_PATH% x86
  goto END

:BUILD
  cl main.cpp eight_input.cpp /EHsc
  if {%errorlevel%} == {0} (
    main.exe
  )
  goto END

:END