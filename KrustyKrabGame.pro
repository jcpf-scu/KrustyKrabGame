#-------------------------------------------------
# KrustyKrabGame - 蟹堡王烹饪模拟游戏
# 项目路径：D:\KrustyKrabGame
# 基于 potato-sandwich-shop 模板简化开发
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KrustyKrabGame
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# 启用 C++11（lambda、范围 for 等语法需要）
CONFIG += c++11

# 源文件列表
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamewindow.cpp \
    menuwindow.cpp \
    introwindow.cpp

# 头文件列表
HEADERS += \
    mainwindow.h \
    gamewindow.h \
    menuwindow.h \
    introwindow.h
