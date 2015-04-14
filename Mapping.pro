#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T13:34:45
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT_QMAKE_EXECUTABLE = /usr/bin/qmake-qt4

CONFIG += console

TARGET = Mapping
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
    mainwindow.cpp \
    cnsmapcanvas.cpp \
    appconfig.cpp \
    db.cpp \
    defs.cpp \
    imagetile.cpp \
    ovrmapcanvas.cpp \
    geometrycalc.cpp \
    spa/spa.c

HEADERS  += mainwindow.h \
    cnsmapcanvas.h \
    appconfig.h \
    db.h \
    defs.h \
    imagetile.h \
    ovrmapcanvas.h \
    geometrycalc.h \
    spa/spa.h 

FORMS    += mainwindow.ui

unix: LIBS += -L/usr/lib/\
 -lgdal \
 -lqgis_core \
 -lqgis_gui \
 -lopencv_core \
 -lopencv_highgui \
 -lopencv_imgproc \
 -lconfig++ \
 -lboost_date_time

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/qgis
DEPENDPATH += /usr/include
DEPENDPATH += /usr/include/qgis

DEFINES += GUI_EXPORT= CORE_EXPORT=
DEFINES += OPENCV_

RESOURCES += \
    resource.qrc
