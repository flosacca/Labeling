QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Labeling
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    dialogs \
    utils \
    widgets \
    windows

HEADERS += \
    dialogs/cuboiddialog.h \
    dialogs/labeldialog.h \
    utils/listex.h \
    utils/util.h \
    widgets/cuboidlabel.h \
    widgets/label.h \
    widgets/renderarea.h \
    windows/mainwindow.h \
    windows/subwindow.h \
    windows/windowfwd.h

SOURCES += \
    dialogs/cuboiddialog.cpp \
    dialogs/labeldialog.cpp \
    main.cpp \
    widgets/cuboidlabel.cpp \
    widgets/label.cpp \
    widgets/renderarea.cpp \
    windows/mainwindow.cpp \
    windows/subwindow.cpp

FORMS += \
    dialogs/cuboiddialog.ui \
    dialogs/labeldialog.ui \
    windows/mainwindow.ui \
    windows/subwindow.ui

RESOURCES += \
    icons.qrc

RC_ICONS = ../res/tag.ico
