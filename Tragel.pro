#-------------------------------------------------
#
# Project created by QtCreator 2019-05-06T13:59:29
#
#-------------------------------------------------

QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tragel
TEMPLATE = app

CONFIG += c++11

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
    src/utils/DarkStyle.cpp \
    src/utils/splashscreen.cpp \
    src/utils/resources.cpp \
    src/widgets/adminsetupdialog.cpp \
    src/widgets/createstaffdialog.cpp \
    src/widgets/centralwindow.cpp \
    src/widgets/userlogindialog.cpp \
    src/widgets/orderwindow.cpp \
    src/widgets/addproductdialog.cpp \
    src/widgets/editproductdialog.cpp \
    src/widgets/allproductsdialog.cpp \
    src/models/productmodel.cpp \
    src/models/productthumbnaildelegate.cpp \
    src/widgets/productuploaddialog.cpp \
    src/widgets/uploadwidget.cpp \
    src/models/ordermodel.cpp \
    src/widgets/orderitemdetaildialog.cpp \
    src/widgets/updateuserdialog.cpp \
    src/widgets/removeuserconfirmationdialog.cpp \
    src/utils/popupnotifier.cpp \
    src/widgets/changepassworddialog.cpp \
    src/widgets/changeuserroledialog.cpp \
    src/widgets/makeorderdialog.cpp \
    src/models/orderingitemmodel.cpp \
    src/widgets/setupdialog.cpp \
    src/widgets/subscribedialog.cpp \
    src/widgets/allsubscriptionsdialog.cpp \
    src/models/allsubscriptionmodel.cpp \
    src/models/usermodel.cpp \
    src/widgets/stockdialog.cpp


HEADERS += \
    include/utils/DarkStyle.h \
    framelesswindow/framelesswindow.h \
    framelesswindow/windowdragger.h \
    include/utils/splashscreen.hpp \
    include/utils/resources.hpp \
    include/widgets/adminsetupdialog.hpp \
    include/widgets/createstaffdialog.hpp \
    include/widgets/centralwindow.hpp \
    include/widgets/userlogindialog.hpp \
    include/widgets/orderwindow.hpp \
    include/widgets/addproductdialog.hpp \
    include/widgets/editproductdialog.hpp \
    include/widgets/allproductsdialog.hpp \
    include/models/productmodel.hpp \
    include/models/productthumbnaildelegate.hpp \
    include/widgets/productuploaddialog.hpp \
    include/widgets/uploadwidget.hpp \
    include/models/ordermodel.hpp \
    include/widgets/orderitemdetaildialog.hpp \
    include/widgets/updateuserdialog.hpp \
    include/widgets/removeuserconfirmationdialog.hpp \
    include/utils/popupnotifier.hpp \
    include/widgets/changepassworddialog.hpp \
    include/widgets/changeuserroledialog.hpp \
    include/widgets/makeorderdialog.hpp \
    include/models/orderingitemmodel.hpp \
    include/widgets/setupdialog.hpp \
    include/widgets/subscribedialog.hpp \
    include/widgets/allsubscriptionsdialog.hpp \
    include/models/allsubscriptionmodel.hpp \
    include/models/usermodel.hpp \
    include/widgets/stockdialog.hpp

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
    forms/removeuserconfirmationdialog.ui \
    forms/changepassworddialog.ui \
    forms/changeuserroledialog.ui \
    forms/makeorderdialog.ui \
    forms/setupdialog.ui \
    forms/subscribedialog.ui \
    forms/allsubscriptionsdialog.ui \
    forms/stockdialog.ui

RESOURCES += \
    darkstyle.qrc \
    framelesswindow.qrc \
    images.qrc
