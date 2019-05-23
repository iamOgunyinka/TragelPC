#-------------------------------------------------
#
# Project created by QtCreator 2019-05-06T13:59:29
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tragel
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += "framelesswindow/"

SOURCES += \
        main.cpp \
    framelesswindow/framelesswindow.cpp \
    framelesswindow/windowdragger.cpp \
    DarkStyle.cpp \
    splashscreen.cpp \
    resources.cpp \
    adminsetupdialog.cpp \
    createstaffdialog.cpp \
    centralwindow.cpp \
    userlogindialog.cpp

HEADERS += \
    DarkStyle.h \
    framelesswindow/framelesswindow.h \
    framelesswindow/windowdragger.h \
    splashscreen.hpp \
    resources.hpp \
    adminsetupdialog.hpp \
    createstaffdialog.hpp \
    centralwindow.hpp \
    userlogindialog.hpp

FORMS += \
    framelesswindow/framelesswindow.ui \
    adminsetupdialog.ui \
    createstaffdialog.ui \
    centralwindow.ui \
    userlogindialog.ui

RESOURCES += \
    darkstyle.qrc \
    framelesswindow.qrc \
    images.qrc
