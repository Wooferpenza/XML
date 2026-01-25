@echo off
REM Переход в корневую директорию проекта
cd /d "%~dp0.."

echo ========================================
echo MinGW Compiler Setup Script
echo ========================================
echo.
echo Этот скрипт поможет настроить MinGW компилятор для проекта.
echo.

echo [INFO] Проверка наличия MinGW компилятора...
echo.

REM Проверка наличия компилятора в PATH
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] MinGW компилятор найден в PATH
    g++ --version
    echo.
) else (
    echo [WARNING] MinGW компилятор не найден в PATH!
    echo.
    echo Для установки MinGW:
    echo 1. Скачайте MinGW-w64 с https://www.mingw-w64.org/
    echo 2. Или установите через Qt Maintenance Tool
    echo 3. Или используйте MSYS2: https://www.msys2.org/
    echo.
    echo После установки добавьте путь к bin в переменную PATH.
    echo Например: C:\Qt\Tools\mingw1120_64\bin
    echo.
)

pause
