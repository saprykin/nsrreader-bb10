APP_NAME = nsrreader_bb10

CONFIG += qt warn_on cascades10
QT += xml

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
