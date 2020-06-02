QT -= gui
QT += core
QT += network mqtt
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
        AJRServer.cpp \
        Configurator.cpp \
         Daemon.cpp \
    Logger.cpp \
    MqttManager.cpp \
        QRServer.cpp \
        ServerConfigurator.cpp \
        TCPServer.cpp \
    db/AJRSale.cpp \
    db/BuildQuery.cpp \
    db/CassTable.cpp \
    db/CodeAccessInfo.cpp \
    db/DBClient.cpp \
    db/DeadTickets.cpp \
    db/Doors.cpp \
    db/FiscUnit.cpp \
    db/MapQVarCass.cpp \
    db/SiteDescriptor.cpp \
    db/SoldAccess.cpp \
    db/Topics.cpp \
        main.cpp \
    TextMetaType.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AJRServer.h \
    Configurator.h \
    Daemon.h \
    Logger.h \
    MqttManager.h \
    QRServer.h \
    ServerConfigurator.h \
    TCPServer.h \
    TextMetaType.h \
    db/AJRSale.h \
    db/BuildQuery.h \
    db/CassTable.h \
    db/CodeAccessInfo.h \
    db/DBClient.h \
    db/DeadTickets.h \
    db/Doors.h \
    db/FiscUnit.h \
    db/MapQVarCass.h \
    db/MapQvarCassCfg.h \
    db/SiteDescriptor.h \
    db/SoldAccess.h \
    db/Topics.h
