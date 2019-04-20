#-------------------------------------------------
#
# Project created by QtCreator 2019-03-02T11:47:49
#
#-------------------------------------------------

QT       += core network gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bee
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#LIBS += -lcurl
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    classroom.cpp \
    word.cpp \
    urlconnection.cpp \
    speaker.cpp \
    statistics.cpp \
    statisticsdialog.cpp \
    dicthelper.cpp \
    wordex.cpp \
    runguard.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    common.h \
    classroom.h \
    word.h \
    urlconnection.h \
    speaker.h \
    statistics.h \
    statisticsdialog.h \
    dicthelper.h \
    wordex.h \
    runguard.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc \
    data.qrc
