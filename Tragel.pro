#-------------------------------------------------
#
# Project created by QtCreator 2019-05-06T13:59:29
#
#-------------------------------------------------

QT       += core gui network websockets charts

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

INCLUDEPATH += "framelesswindow/"\
                "include/"

SOURCES += \
        src/main.cpp \
    framelesswindow/framelesswindow.cpp \
    framelesswindow/windowdragger.cpp \
    src/DarkStyle.cpp \
    src/splashscreen.cpp \
    src/resources.cpp \
    src/adminsetupdialog.cpp \
    src/createstaffdialog.cpp \
    src/centralwindow.cpp \
    src/userlogindialog.cpp \
    src/orderwindow.cpp \
    src/addproductdialog.cpp \
    src/editproductdialog.cpp \
    src/allproductsdialog.cpp \
    src/productmodel.cpp \
    src/productthumbnaildelegate.cpp \
    src/productuploaddialog.cpp \
    src/uploadwidget.cpp \
    src/ordermodel.cpp \
    src/orderitemdetaildialog.cpp \
    src/updateuserdialog.cpp \
    src/removeuserconfirmationdialog.cpp \
    src/popupnotifier.cpp

HEADERS += \
    include/DarkStyle.h \
    framelesswindow/framelesswindow.h \
    framelesswindow/windowdragger.h \
    include/splashscreen.hpp \
    include/resources.hpp \
    include/adminsetupdialog.hpp \
    include/createstaffdialog.hpp \
    include/centralwindow.hpp \
    include/userlogindialog.hpp \
    include/orderwindow.hpp \
    include/addproductdialog.hpp \
    include/editproductdialog.hpp \
    include/allproductsdialog.hpp \
    include/productmodel.hpp \
    include/productthumbnaildelegate.hpp \
    include/productuploaddialog.hpp \
    include/uploadwidget.hpp \
    include/ordermodel.hpp \
    include/orderitemdetaildialog.hpp \
    include/updateuserdialog.hpp \
    include/removeuserconfirmationdialog.hpp \
    include/popupnotifier.hpp

FORMS += \
    framelesswindow/framelesswindow.ui \
    forms/adminsetupdialog.ui \
    forms/createstaffdialog.ui \
    forms/centralwindow.ui \
    forms/userlogindialog.ui \
    forms/orderwindow.ui \
    forms/addproductdialog.ui \
    forms/editproductdialog.ui \
    forms/allproductsdialog.ui \
    forms/productuploaddialog.ui \
    forms/uploadwidget.ui \
    forms/orderitemdetaildialog.ui \
    forms/updateuserdialog.ui \
    forms/removeuserconfirmationdialog.ui

RESOURCES += \
    darkstyle.qrc \
    framelesswindow.qrc \
    images.qrc
