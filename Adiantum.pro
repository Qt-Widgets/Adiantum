#-------------------------------------------------
#
# Project created by QtCreator 2018-08-23T19:46:56
#
#-------------------------------------------------

QT       += core gui network
CONFIG   += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Adiantum
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    adiantum.cpp \
    element.cpp \
    webloader.cpp

HEADERS  += \
    adiantum.h \
    element.h \
    webloader.h \
    ./lib/lua/include/lua.h \
    ./lib/lua/include/lauxlib.h \
    ./lib/lua/include/lualib.h \
    ./lib/sol/sol.hpp

RESOURCES += \
    resources.qrc

LIBS += -L$$PWD/lib/lua/ -llua53 \

INCLUDEPATH += $$PWD/lib/lua/include \
               $$PWD/lib/sol
DEPENDPATH += $$PWD/lib/lua/include \
              $$PWD/lib/sol
