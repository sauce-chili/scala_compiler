@echo off
setlocal EnableExtensions EnableDelayedExpansion

:: Проверка входного каталога
if "%~1"=="" (
    echo Usage: merge_cpp.bat ^<source_folder^> [output_file]
    exit /b 1
)

set "SRC_DIR=%~1"
for %%I in ("%SRC_DIR%") do set "SRC_DIR_FULL=%%~fI"

if not exist "%SRC_DIR%" (
    echo Source directory does not exist: %SRC_DIR%
    exit /b 1
)

:: Выходной файл
if "%~2"=="" (
    set "OUT_FILE=%CD%\all-merged.txt"
) else (
    set "OUT_FILE=%~2"
)

:: Очистка выходного файла
> "%OUT_FILE%" echo Merged C++ files
>> "%OUT_FILE%" echo Source directory: %SRC_DIR_FULL%
>> "%OUT_FILE%" echo Excluded directory: cmake-build-debug
>> "%OUT_FILE%" echo ============================================
>> "%OUT_FILE%" echo(

:: Расширения C++
set EXTENSIONS=cpp cxx cc c h hpp hxx

for %%E in (%EXTENSIONS%) do (
    for /R "%SRC_DIR%" %%F in (*.%%E) do (

        :: Проверка на исключённый каталог
        echo %%~fF | findstr /I "\\cmake-build-debug\\" >nul
        if errorlevel 1 (
            echo ===== FILE: %%~fF =====>>"%OUT_FILE%"
            type "%%~fF">>"%OUT_FILE%"
            echo(>>"%OUT_FILE%"
            echo(>>"%OUT_FILE%"
        )
    )
)

echo Done.
echo Output file: %OUT_FILE%
endlocal
