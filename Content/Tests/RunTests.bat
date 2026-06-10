@echo off
setlocal
cd /d "%~dp0..\..\..\..\.."
set "APP=Game\Binaries\Win64\WidgetMarkupApp.exe"
set "UPROJECT=%~1"
if "%UPROJECT%"=="" set "UPROJECT=%CD%\Game\Game.uproject"

echo ========================================
echo WidgetMarkup Test Suite
echo ========================================

echo [1/4] Python framework tests...
"%APP%" /WidgetMarkup/Tests/TestPythonFramework --project "%UPROJECT%"
if %errorlevel% neq 0 goto :fail

echo [2/4] Blueprint compilation tests...
"%APP%" /WidgetMarkup/Tests/TestBlueprintCompilation --project "%UPROJECT%"
if %errorlevel% neq 0 goto :fail

echo [3/4] Style inline tests...
"%APP%" /WidgetMarkup/Tests/TestStyleInline --project "%UPROJECT%"
if %errorlevel% neq 0 goto :fail

echo [4/4] Style override tests...
"%APP%" /WidgetMarkup/Tests/TestStyleOverride --project "%UPROJECT%"
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
