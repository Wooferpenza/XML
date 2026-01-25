QT += core widgets xml

CONFIG += c++17

TARGET = MyQtApp
TEMPLATE = app

# Исходные файлы
SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Настройки для Windows
win32 {
    # Путь к Qt (опционально, если Qt в PATH)
    # QT_INSTALL_PREFIX = C:/Qt/6.8.1/mingw_64
    
    # Настройки компилятора MinGW
    QMAKE_CXX = g++
    QMAKE_CC = gcc
    QMAKE_LINK = g++
    
    # Флаги компилятора
    QMAKE_CXXFLAGS += -Wall -Wextra
    QMAKE_CFLAGS += -Wall -Wextra
    
    # Оптимизация для release
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS_RELEASE += -O2
        QMAKE_CFLAGS_RELEASE += -O2
    }
    
    # Отладочная информация для debug
    CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS_DEBUG += -g
        QMAKE_CFLAGS_DEBUG += -g
    }
}

# Настройки компилятора
CONFIG += warn_on

# Версия приложения
VERSION = 1.0.0
