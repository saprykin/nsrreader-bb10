APP_NAME = nsrreader_bb10_lite

CONFIG += qt warn_on cascades10
QT += xml

DEFINES += HAVE_CONFIG_H
DEFINES += NSR_LITE_VERSION

INCLUDEPATH += $$_PRO_FILE_PWD_/src/poppler

LIBS += -lpng
LIBS += -liconv
LIBS += -lcurl
LIBS += -ljpeg
LIBS += -lfontconfig
LIBS += -lfreetype
LIBS += -lz
LIBS += -lbbcascadespickers
LIBS += -lbb
LIBS += -lbbsystem
LIBS += -lbbutilityi18n
LIBS += -lbbutility
LIBS += -lbbdevice
LIBS += -lbbmultimedia

include(config.pri)
