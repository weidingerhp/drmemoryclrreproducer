@ECHO OFF
pushd %~dp0

REM Change PLATFORM either to win32 or x64 or ALL - depending on your desired Platform.
set PLATFORM=x86
REM Change TARGET to Release or Debug - depending what you want to build
set TARGET=Debug


set MSBUILD="%ProgramFiles(x86)%"\MSBuild\14.0\Bin\MSBuild.exe
if not exist %MSBUILD% (
  set MSBUILD="%WINDIR%\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe"
)

%MSBUILD% TestDrMemory.sln /l:FileLogger,Microsoft.Build.Engine;logfile=build.log /m:4 /p:Platform=%PLATFORM% /p:Configuration=%TARGET% /t:TestDrMemory
if not %ERRORLEVEL%==0 goto :error

REM Version with drmemory hangs and shutdown
rem <path to drmemory>\bin\drmemory.exe -- %TARGET%\TestDrMemory.exe

%TARGET%\TestDrMemory.exe
pause

goto :end
:error
  pause BUILD FAILED!! .....
  start build.log

:end

popd