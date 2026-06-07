@echo off
setlocal
cd /d "%~dp0..\..\..\..\.."
set "APP=Game\Binaries\Win64\WidgetMarkupApp.exe"
set "UPROJECT=%~1"
if "%UPROJECT%"=="" set "UPROJECT=%CD%\Game\Game.uproject"

echo ========================================
echo WidgetMarkup Test Suite
echo ========================================

echo [1/2] Unit tests...
"%APP%" /WidgetMarkup/Tests/UnitTests --project "%UPROJECT%"
if %errorlevel% neq 0 goto :fail

echo [2/2] Integration tests...
"%APP%" /WidgetMarkup/Tests/TestCases --project "%UPROJECT%"
if %errorlevel% neq 0 goto :fail

echo.
echo ========================================
echo All tests passed.
echo ========================================
goto :end

:fail
echo.
echo ========================================
echo TESTS FAILED
echo ========================================
pause

:end
pause
