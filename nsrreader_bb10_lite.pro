APP_NAME = nsrreader_bb10_lite

CONFIG += qt warn_on cascades10
QT += xml

DEFINES += NSR_LITE_VERSION
INCLUDEPATH += ../nsrreadercore

LIBS += -L$$quote($$_PRO_FILE_PWD_)/assets/lib

LIBS += -lbbcascadespickers
LIBS += -lbb
LIBS += -lbbsystem
LIBS += -lbbutilityi18n
LIBS += -lbbutility
LIBS += -lbbdevice
LIBS += -lbbmultimedia
LIBS += -lnsrreadercore

include (config.pri)
