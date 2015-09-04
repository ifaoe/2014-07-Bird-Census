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


VPATH += ./src
SOURCES += main.cpp\
    mainwindow.cpp \
    cnsmapcanvas.cpp \
    appconfig.cpp \
    db.cpp \
    defs.cpp \
    imagetile.cpp \
    ovrmapcanvas.cpp \
    QgsMapMarker.cpp \
    QtExtension/QFrozenRowTableWidget.cpp 

HEADERS  += mainwindow.h \
    cnsmapcanvas.h \
    appconfig.h \
    db.h \
    defs.h \
    imagetile.h \
    ovrmapcanvas.h \
    QgsMapMarker.h \
    QtExtension/QFrozenRowTableWidget.h

VPATH += ./ui
FORMS    += mainwindow.ui

unix: LIBS += -L/usr/lib/\
 -lqgis_core \
 -lqgis_gui \
 -lconfig++ \
 -lboost_date_time
 
RESOURCES += ui/style.qrc

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/qgis
DEPENDPATH += /usr/include
DEPENDPATH += /usr/include/qgis

DEFINES += GUI_EXPORT= CORE_EXPORT=
