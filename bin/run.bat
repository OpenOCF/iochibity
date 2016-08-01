@echo off
REM Helper script to build and run IoTivity on Windows
SETLOCAL ENABLEDELAYEDEXPANSION

if [%1]==[] goto USAGE

set IOTIVITY_DIR=%~dp0
set HOME=%USERPROFILE%

IF "%1" == "msys" (
  set BUILD_MSYS="YES"
  SHIFT
) ELSE (
  set BUILD_MSYS=
)

set CURRENT_ARG=%1
set SECOND_ARG=%2
set DEBUG=

if "!SECOND_ARG!"=="debug" (
  set DEBUG="%ProgramFiles(x86)%\Windows Kits\10\Debuggers\x64\cdb.exe" -2 -c "g" 
)

IF "%CURRENT_ARG%" == "build" IF "%BUILD_MSYS%" == "" (
  IF NOT "%VS140COMNTOOLS%" == "" (
    CALL "%VS140COMNTOOLS%"vsvars32.bat
  ) ELSE (
    IF NOT "%VS120COMNTOOLS%" == "" (
      CALL "%VS120COMNTOOLS%"vsvars32.bat
      )
    )

  IF NOT "!VSINSTALLDIR!" == "" (
      CALL "!VSINSTALLDIR!VC\vcvarsall.bat" amd64
  ) ELSE (
    @ECHO WARNING: Could not find vsvarsall.bat.
    @ECHO WARNING: VISUAL STUDIO 2013/2015 DOES NOT APPEAR TO BE INSTALLED ON THIS MACHINE
    GOTO :EOF
  )
)

REM We need to append the "PATH" so the octbstack.dll can be found by executables
IF "%BUILD_MSYS%" == "" (
  set BUILD_DIR=out\windows\amd64\debug
  set PATH=!PATH!;!BUILD_DIR!;
) ELSE (
  set BUILD_DIR=out\msys_nt\x86_64\debug
  set PATH=!PATH!;!BUILD_DIR!;C:\msys64\mingw64\bin
)

REM *** BUILD OPTIONS ***
set TARGET_OS=windows
set TARGET_ARCH=amd64
set SECURED=1
set TEST=1
set LOGGING=OFF
set WITH_RD=1
REM *** BUILD OPTIONS ***

if "!CURRENT_ARG!"=="server" (
  %DEBUG% %BUILD_DIR%\resource\examples\simpleserver.exe
) else if "!CURRENT_ARG!"=="client" (
  %DEBUG% %BUILD_DIR%\resource\examples\simpleclient.exe
) else if "!CURRENT_ARG!"=="mediaclient" (
  %DEBUG% %BUILD_DIR%\debug\resource\examples\mediaclient.exe
) else if "!CURRENT_ARG!"=="mediaserver" (
  %DEBUG% %BUILD_DIR%\resource\examples\mediaserver.exe
) else if "!CURRENT_ARG!"=="winuiclient" (
  %DEBUG% %BUILD_DIR%\resource\examples\winuiclient.exe
) else if "!CURRENT_ARG!"=="occlient" (
  %DEBUG% %BUILD_DIR%\resource\csdk\stack\samples\linux\SimpleClientServer\occlientbasicops.exe -u 0 -t 3 -c 1
) else if "!CURRENT_ARG!"=="ocserver" (
  %DEBUG% %BUILD_DIR%\resource\csdk\stack\samples\linux\SimpleClientServer\ocserverbasicops.exe
) else if "!CURRENT_ARG!"=="test" (
  %DEBUG% %BUILD_DIR%\resource\csdk\connectivity\test\catests.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\stack\test\stacktests.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\stack\test\cbortests.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\security\unittest\unittest.exe
  %DEBUG% %BUILD_DIR%\resource\csdk\security\provisioning\unittest\unittest.exe
) else if "!CURRENT_ARG!"=="build" (
  echo Starting IoTivity build with these options:
  echo   TARGET_OS=%TARGET_OS%
  echo   TARGET_ARCH=%TARGET_ARCH%
  echo   SECURED=%SECURED%
  echo   LOGGING=%LOGGING%
  echo   WITH_RD=%WITH_RD%
  CL.exe | findstr "Compiler Verison"
  echo.scons VERBOSE=1 TARGET_OS=%TARGET_OS% TARGET_ARCH=%TARGET_ARCH% RELEASE=0 WITH_RA=0 TARGET_TRANSPORT=IP SECURED=%SECURED% WITH_TCP=0 BUILD_SAMPLE=ON LOGGING=%LOGGING% TEST=%TEST% WITH_RD=%WITH_RD%
  scons VERBOSE=1 TARGET_OS=%TARGET_OS% TARGET_ARCH=%TARGET_ARCH% RELEASE=0 WITH_RA=0 TARGET_TRANSPORT=IP SECURED=%SECURED% WITH_TCP=0 BUILD_SAMPLE=ON LOGGING=%LOGGING% TEST=%TEST% WITH_RD=%WITH_RD%
) else if "!CURRENT_ARG!"=="clean" (
  scons VERBOSE=1 TARGET_OS=%TARGET_OS% TARGET_ARCH=%TARGET_ARCH% RELEASE=0 WITH_RA=0 TARGET_TRANSPORT=IP SECURED=%SECURED% WITH_TCP=0 BUILD_SAMPLE=ON LOGGING=%LOGGING% TEST=%TEST% WITH_RD=%WITH_RD% -c clean
  rd /s /q out
  del .sconsign.dblite
) else if "!CURRENT_ARG!"=="cleangtest" (
  rd /s /q extlibs\gtest\gtest-1.7.0
  del extlibs\gtest\gtest-1.7.0.zip
) else (
    echo %0 - Script requires a valid argument!
    goto :EOF
)

echo Done!

goto EOF

:USAGE
echo %0 - Helper to build/test iotivity.  Requires an argument.
echo Installation: Drop this into your iotivity root directory to use it.
echo.
echo. Usage examples:
echo   Launch SimpleClient with debugger:
echo      %0 client debug
echo.
echo   Launch SimpleServer:
echo      %0 server
echo.
echo   Launch WinUIClient built in msys:
echo      %0 msys winuiclient
echo.
echo   Build:
echo      %0 build
echo.
echo   Run all tests:
echo      %0 test
echo.
echo   Clean:
echo      %0 clean

:EOF
