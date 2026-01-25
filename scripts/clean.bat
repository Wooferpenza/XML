@echo off
REM Переход в корневую директорию проекта
cd /d "%~dp0.."

echo ========================================
echo Qt Application Clean Script
echo ========================================
echo.

REM Установка путей к Qt
set "QT_DIR=C:\Qt\6.8.1\mingw_64"
set "PATH=%QT_DIR%\bin;%PATH%"

REM Поиск MinGW компилятора
set "MINGW_PATH="
if exist "C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe" (
    set "MINGW_PATH=C:\Qt\Tools\mingw1120_64\bin"
    goto :mingw_found
)
if exist "C:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe" (
    set "MINGW_PATH=C:\Qt\Tools\mingw1310_64\bin"
    goto :mingw_found
)
if exist "C:\Qt\Tools\mingw810_64\bin\mingw32-make.exe" (
    set "MINGW_PATH=C:\Qt\Tools\mingw810_64\bin"
    goto :mingw_found
)
if exist "C:\mingw64\bin\mingw32-make.exe" (
    set "MINGW_PATH=C:\mingw64\bin"
    goto :mingw_found
)

:mingw_found
if not "%MINGW_PATH%"=="" (
    set "PATH=%MINGW_PATH%;%PATH%"
)

if exist Makefile (
    echo [INFO] Очистка скомпилированных файлов...
    mingw32-make clean >nul 2>&1
    echo.
)

REM Удаление папок сборки
if exist debug (
    echo [INFO] Удаление папки debug...
    rmdir /s /q debug 2>nul
)

if exist release (
    echo [INFO] Удаление папки release...
    rmdir /s /q release 2>nul
)

REM Удаление файлов сборки
if exist Makefile (
    del /q Makefile 2>nul
)
if exist Makefile.Debug (
    del /q Makefile.Debug 2>nul
)
if exist Makefile.Release (
    del /q Makefile.Release 2>nul
)
if exist *.obj (
    del /q *.obj 2>nul
)

echo [SUCCESS] Очистка завершена!
echo.

pause
