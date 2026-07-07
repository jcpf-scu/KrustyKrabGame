#-------------------------------------------------
# KrustyKrabGame project file
# Project path: D:\KrustyKrabGame
# Based on potato-sandwich-shop template
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KrustyKrabGame
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# Enable C++11 support
CONFIG += c++11

# Source files
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamewindow.cpp \
    menuwindow.cpp \
    introwindow.cpp

# Header files
HEADERS += \
    mainwindow.h \
    gamewindow.h \
    menuwindow.h \
    introwindow.h

RESOURCES += \
    images.qrc

