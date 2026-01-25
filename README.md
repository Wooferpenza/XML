# Qt Application Project

Проект создан с использованием Qt 6.8.1 и qmake.

## Требования

- Qt 6.8.1 (mingw_64) установлен в `C:\Qt\6.8.1\mingw_64`
- MinGW компилятор (g++, gcc, mingw32-make)
- qmake (входит в состав Qt)

### Установка MinGW компилятора

Если компилятор не установлен, выполните:

```bash
scripts\setup_compiler.bat
```

Этот скрипт поможет найти или установить MinGW компилятор.

**Варианты установки:**
1. Через Qt Maintenance Tool (рекомендуется)
2. Скачать MinGW-w64 с https://www.mingw-w64.org/
3. Использовать MSYS2: https://www.msys2.org/

После установки добавьте путь к `bin` папке MinGW в переменную окружения PATH.

## Сборка проекта

### Быстрая сборка (рекомендуется):

**Windows (BAT скрипты):**
```bash
scripts\build.bat    # Сборка проекта
scripts\run.bat      # Запуск приложения
scripts\clean.bat    # Очистка скомпилированных файлов
```

### Ручная сборка через командную строку:

1. Откройте командную строку и перейдите в папку проекта:
```bash
cd d:\Project\XML
```

2. Настройте переменную окружения PATH (если qmake не в PATH):
```bash
set PATH=C:\Qt\6.8.1\mingw_64\bin;%PATH%
```

3. Создайте Makefile:
```bash
qmake MyQtApp.pro
```

4. Соберите проект:
```bash
mingw32-make
```

Или используйте nmake для MSVC:
```bash
nmake
```

5. Запустите приложение:
```bash
.\release\MyQtApp.exe
```
или
```bash
.\debug\MyQtApp.exe
```

### Через Qt Creator:

1. Откройте Qt Creator
2. File -> Open File or Project
3. Выберите `MyQtApp.pro`
4. Qt Creator автоматически определит настройки Qt
5. Нажмите Configure Project
6. Запустите проект (Ctrl+R)

### Очистка проекта:

Для очистки скомпилированных файлов:
```bash
mingw32-make clean
```

Или удалите папки `debug` и `release` вручную.

## Структура проекта

```
XML/
├── MyQtApp.pro          # Файл конфигурации qmake
├── main.cpp             # Точка входа приложения
├── mainwindow.h         # Заголовочный файл главного окна
├── mainwindow.cpp       # Реализация главного окна
├── mainwindow.ui        # Файл интерфейса (Qt Designer)
├── MyQtApp_resource.rc  # Файл ресурсов Windows
├── scripts/             # Папка со скриптами сборки
│   ├── build.bat        # Скрипт сборки проекта (автоматически находит компилятор)
│   ├── run.bat          # Скрипт запуска приложения
│   ├── clean.bat        # Скрипт очистки скомпилированных файлов
│   └── setup_compiler.bat # Скрипт для настройки/поиска MinGW компилятора
├── README.md            # Документация проекта
└── .gitignore          # Игнорируемые файлы Git
```

### Скрипты сборки:

Все скрипты находятся в папке `scripts/`:

- `scripts\build.bat` - сборка проекта (автоматически находит компилятор)
- `scripts\run.bat` - запуск приложения
- `scripts\clean.bat` - очистка скомпилированных файлов
- `scripts\setup_compiler.bat` - настройка/поиск MinGW компилятора

## Примечания

При запуске приложения убедитесь, что DLL файлы Qt находятся в PATH или скопируйте их из `C:\Qt\6.8.1\mingw_64\bin` в папку с исполняемым файлом.
