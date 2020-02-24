QT -= gui
QT += core
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
EXT_PATH = $${PWD}/../external/lib
INCLUDEPATH += $${EXT_PATH}/include
LIBS += -L$${EXT_PATH}

LIBS        += -lcassandra_static -lz -lssl -lcrypto -luv

SOURCES += \
        AJRSale.cpp \
        AJRServer.cpp \
        BuildQuerry.cpp \
        CassTable.cpp \
        Configurator.cpp \
        DBClient.cpp \
        Daemon.cpp \
        QRServer.cpp \
        ServerConfigurator.cpp \
        TCPServer.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AJRSale.h \
    AJRServer.h \
    BuildQuerry.h \
    CassTable.h \
    Configurator.h \
    DBClient.h \
    Daemon.h \
    QRServer.h \
    ServerConfigurator.h \
    TCPServer.h
