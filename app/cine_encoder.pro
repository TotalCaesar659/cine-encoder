QT += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    dialog.cpp \
    donate.cpp \
    main.cpp \
    mainwindow.cpp \
    preset.cpp \
    selectpreset.cpp \
    settings.cpp \
    taskcomplete.cpp

HEADERS += \
    about.h \
    dialog.h \
    donate.h \
    mainwindow.h \
    preset.h \
    selectpreset.h \
    settings.h \
    taskcomplete.h

FORMS += \
    about.ui \
    dialog.ui \
    donate.ui \
    mainwindow.ui \
    preset.ui \
    selectpreset.ui \
    settings.ui \
    taskcomplete.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    files.qrc

unix:!macx: LIBS += -lmediainfo

# RC_FILE = icon.rc
