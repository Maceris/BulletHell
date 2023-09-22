@ECHO OFF

set platform=x64
set configuration=Debug

.\dist\%platform%\%configuration%\Asset_Packer.exe

IF %ERRORLEVEL% NEQ 0 (
  ECHO ----
  ECHO Could not find asset packer for the %platform% %configuration% build, please check the script
)
