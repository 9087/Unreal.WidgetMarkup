@echo off
setlocal
cd /d "%~dp0..\..\..\..\.."
set "APP=Game\Binaries\Win64\WidgetMarkupApp.exe"
set "EXTRA_ARGS=--extra-arguments test"
set "UPROJECT=%~1"
if "%UPROJECT%"=="" set "UPROJECT=%CD%\Game\Game.uproject"

echo ========================================
echo WidgetMarkup Test Suite
echo ========================================

set /a COUNT=0

echo [1/17] Empty widgetmarkup...
"%APP%" /WidgetMarkup/Tests/TestEmpty --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [2/17] Reactive properties...
"%APP%" /WidgetMarkup/Tests/TestReactive --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [3/17] Computed properties...
"%APP%" /WidgetMarkup/Tests/TestComputed --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [4/17] TextBlock widget...
"%APP%" /WidgetMarkup/Tests/TestTextBlock --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [5/17] Button widget...
"%APP%" /WidgetMarkup/Tests/TestButton --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [6/17] Image widget...
"%APP%" /WidgetMarkup/Tests/TestImage --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [7/17] Border widget (single-child container)...
"%APP%" /WidgetMarkup/Tests/TestBorder --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [8/17] CanvasPanel widget...
"%APP%" /WidgetMarkup/Tests/TestCanvasPanel --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [9/17] HorizontalBox widget...
"%APP%" /WidgetMarkup/Tests/TestHorizontalBox --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [10/17] VerticalBox widget...
"%APP%" /WidgetMarkup/Tests/TestVerticalBox --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [11/17] Overlay widget...
"%APP%" /WidgetMarkup/Tests/TestOverlay --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [12/17] StyleSheet (inline)...
"%APP%" /WidgetMarkup/Tests/TestStyleSheetInline --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [13/17] StyleSheet (inherit standalone file + override)...
"%APP%" /WidgetMarkup/Tests/TestStyleSheetOverride --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [14/17] ListView + ObservableCollection...
"%APP%" /WidgetMarkup/Tests/TestListView --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [15/17] Dynamic add_child / remove_child / get_child...
"%APP%" /WidgetMarkup/Tests/TestDynamicChild --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [16/17] Static child widget blueprint + get_child / remove_child...
"%APP%" /WidgetMarkup/Tests/TestStaticChild --project "%UPROJECT%" %EXTRA_ARGS%
if %errorlevel% neq 0 goto :fail

echo [17/17] Variable element (defaults, types, brace literals)...
"%APP%" /WidgetMarkup/Tests/TestVariable --project "%UPROJECT%" %EXTRA_ARGS%
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

:end
