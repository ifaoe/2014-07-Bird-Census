#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T13:34:45
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mapping
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
    mainwindow.cpp \
    cnsmapcanvas.cpp \
    textlogger.cpp \
    appconfig.cpp \
    sqlquery.cpp \
    db.cpp \
    defs.cpp \
    imagetile.cpp \
    ovrmapcanvas.cpp

HEADERS  += mainwindow.h \
    cnsmapcanvas.h \
    textlogger.h \
    appconfig.h \
    sqlquery.h \
    db.h \
    defs.h \
    imagetile.h \
    ovrmapcanvas.h

FORMS    += mainwindow.ui

unix: LIBS += -L/usr/lib/\
 -lgdal \
 -lqgis_core \
 -lqgis_gui \
 -lopencv_core \
 -lopencv_highgui \
 -lopencv_imgproc \
 -lconfig++

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/qgis
DEPENDPATH += /usr/include
DEPENDPATH += /usr/include/qgis

DEFINES += GUI_EXPORT= CORE_EXPORT=
DEFINES += OPENCV_

RESOURCES += \
    resource.qrc
