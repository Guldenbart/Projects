#-------------------------------------------------
#
# Project created by QtCreator 2016-08-14T23:00:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dexiv
TEMPLATE = app


SOURCES += main.cpp \
    protowindow.cpp \
    personSquare.cpp \
    myImageLabel.cpp \
    imageinfo.cpp

HEADERS += protowindow.h \
    personSquare.h \
    myImageLabel.h \
    imageinfo.h

FORMS    += protowindow.ui

win32 {
#header
#INCLUDEPATH += $$quote(C:/MinGW/msys/1.0/local/include)
#INCLUDEPATH += $$quote(C:/MinGW/msys/1.0/local/include/exiv2)
#libs
#LIBS += $$quote(C:/MinGW/msys/1.0/local/lib/libexpat.dll.a)
#LIBS += $$quote(C:/MinGW/msys/1.0/local/lib/libz.dll.a)
#LIBS += $$quote(C:/MinGW/msys/1.0/local/lib/libexiv2.dll.a)
}
