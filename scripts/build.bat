@echo off
REM Переход в корневую директорию проекта
cd /d "%~dp0.."

echo ========================================
echo Qt Application Build Script
echo ========================================
echo.

REM Установка путей к Qt
set "QT_DIR=C:\Qt\6.8.1\mingw_64"
set "PATH=%QT_DIR%\bin;%PATH%"

REM Поиск MinGW компилятора
set "MINGW_PATH="
if exist "C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe" (
    set "MINGW_PATH=C:\Qt\Tools\mingw1120_64\bin"
 REM   goto :mingw_found
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
if exist "C:\mingw32\bin\mingw32-make.exe" (
    set "MINGW_PATH=C:\mingw32\bin"
    goto :mingw_found
)

:mingw_found
if not "%MINGW_PATH%"=="" (
    echo [INFO] Найден MinGW: %MINGW_PATH%
    set "PATH=%MINGW_PATH%;%PATH%"
) else (
    echo [WARNING] MinGW не найден в стандартных местах
    echo [WARNING] Проверяю наличие mingw32-make в PATH...
    where mingw32-make >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] mingw32-make не найден!
        echo Установите MinGW или добавьте его в PATH.
        pause
        exit /b 1
    )
)

REM Проверка наличия qmake
where qmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] qmake не найден в PATH!
    echo Убедитесь, что Qt установлен в %QT_DIR%
    pause
    exit /b 1
)

echo [INFO] Найден qmake:
qmake -v
echo.

REM Генерация Makefile
echo [INFO] Генерация Makefile...
call qmake MyQtApp.pro
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Ошибка при генерации Makefile!
    pause
    exit /b 1
)

REM Сборка проекта
echo [INFO] Сборка проекта...
echo.
mingw32-make
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Ошибка при сборке проекта!
    pause
    exit /b 1
)

echo.
echo ========================================
echo [SUCCESS] Сборка завершена успешно!
echo ========================================
echo.
echo Исполняемый файл находится в:
if exist release\MyQtApp.exe (
    echo   - release\MyQtApp.exe
) else if exist debug\MyQtApp.exe (
    echo   - debug\MyQtApp.exe
)
echo.

REM pause
