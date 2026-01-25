@echo off
REM Переход в корневую директорию проекта
cd /d "%~dp0.."

echo ========================================
echo Qt Application Run Script
echo ========================================
echo.

REM Установка путей к Qt (для DLL)
set "QT_DIR=C:\Qt\6.8.1\mingw_64"
set "PATH=%QT_DIR%\bin;%PATH%"

REM Поиск исполняемого файла
if exist release\MyQtApp.exe (
    echo [INFO] Запуск Release версии...
    start "" "release\MyQtApp.exe"
    echo [INFO] Приложение запущено.
) else if exist debug\MyQtApp.exe (
    echo [INFO] Запуск Debug версии...
    start "" "debug\MyQtApp.exe"
    echo [INFO] Приложение запущено.
) else (
    echo [ERROR] Исполняемый файл не найден!
    echo.
    echo Сначала выполните сборку проекта:
    echo   build.bat
    echo.
    pause
    exit /b 1
)

echo.
