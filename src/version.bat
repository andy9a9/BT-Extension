@echo off

setlocal

:: overim, ci su zadane vsetky vstupne parametre
set PROJECT_DIR=%1
:: cesta ku vstupnemu HEX suboru
set INPUT_FILE=%2

:: spustenie priamo z adresara
if "%PROJECT_DIR%." == "." set "PROJECT_DIR=%cd%"
if "%INPUT_FILE%." == "." (
   for %%i in (%PROJECT_DIR%\out\*.hex) do set INPUT_FILE="%%~fi"
)

:: zistim, ci subor vstupny subor existuje
if NOT EXIST "%INPUT_FILE%" goto Missing
:: zistim, ci ide o HEX subor
if NOT "%INPUT_FILE:~-4,3%" == "hex" goto Missing
call :removeX PROJECT_DIR

echo Signing output HEX file... | tr -d "\r\n"

:: defaultne oznacenie verzie
set NAZOV_PROJ=main
set VERZIA_SW=1
set PODVERZIA_SW=255

:: cesta k identifikacnemu suboru
set IDENT_FILE="%PROJECT_DIR%\ident.h"
:: zistim, ci existuje informacny subor
if NOT EXIST "%IDENT_FILE%" goto deff

:: ziskam nazov projektu
call :getLastString PROJECT_DIR

:: ziskam cislo verzie
for /f "delims=" %%a in ('grep -m 1 "VERZIA_SW" %IDENT_FILE% ^| grep -o [0-9]') do set VERZIA_SW=%%a
for /f "delims=" %%a in ('grep -m 1 "PODVERZIA_SW" %IDENT_FILE% ^| grep -o [0-9]') do set PODVERZIA_SW=%%a
rem echo verzia:%VERZIA_SW% podverzia:%PODVERZIA_SW%

:deff
:: zistim, ci HEX obsahuje datum vytvorenia
for /f "delims=" %%a in ('grep -o "CREATED" %INPUT_FILE%') do set WAS_CREATED=%%a
if NOT "%WAS_CREATED%"=="" (
   rem odstranim text za CREATED, vratane
   sed "s/CREATED.*//g" %INPUT_FILE% > %INPUT_FILE%.tmp
   move /Y %INPUT_FILE%.tmp %INPUT_FILE% > NUL
   goto Version
)

:: zistim, ci HEX obsahuje cislo verzie
for /f "delims=" %%a in ('grep -o "v" %INPUT_FILE%') do set OLD_VERSION=%%a
if NOT "%OLD_VERSION%"=="" (
   rem odstranim povodne oznacenie verzie
   sed "/v/d" %INPUT_FILE% > %INPUT_FILE%.tmp
   move /Y %INPUT_FILE%.tmp %INPUT_FILE% > NUL
)

:Version
:: pridam na koniec cislo verzie, datum a cas kompilacie
for /f "skip=1" %%x in ('wmic os get localdatetime') do if not defined NEW_DATE set NEW_DATE=%%x
for /f "delims=" %%a in ('time /t') do set COMPILE_TIME=%%a
set COMPILE_DATETIME=%NEW_DATE:~0,4%-%NEW_DATE:~4,2%-%NEW_DATE:~6,2% %COMPILE_TIME%

:: zapisem vyskladany retazec do HEX suboru
::echo ;%NAZOV_PROJ% v%VERZIA_SW%.%PODVERZIA_SW% %COMPILE_DATE%
echo ;%NAZOV_PROJ% v%VERZIA_SW%.%PODVERZIA_SW% %COMPILE_DATETIME% >> %INPUT_FILE%

echo OK
goto End

:Missing
echo HEX file is missing!
goto End

:Usage
echo Input arguments ERROR!

:End
endlocal
goto :eof

:: funkcia upravi cestu ku projektu
:removeX PROJECT_DIR
:: ak 3. znak je '.', tak odstran .X
if "%PROJECT_DIR:~-3,1%" == "." set PROJECT_DIR=%PROJECT_DIR:~0,-3%
:: odstran "" okolo premennej
set PROJECT_DIR=%PROJECT_DIR:"=%
goto :eof

:: funkcia vrati nazov projektu
:getLastString PROJECT_DIR
for /f "tokens=*" %%i in ('echo %PROJECT_DIR%') do set NAZOV_PROJ=%%~ni
goto :eof
