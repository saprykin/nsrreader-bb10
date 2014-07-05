APP_NAME = nsrreader_bb10_lite

CONFIG += qt warn_on cascades10
QT += xml

include ($$_PRO_FILE_PWD_/src/nsrreadercore/nsrreadercore_blackberry.pri)

DEFINES += NSR_LITE_VERSION
DEFINES += $$NSRREADERCORE_DEFINES

INCLUDEPATH += $$_PRO_FILE_PWD_/src/nsrreadercore/3dparty/poppler
INCLUDEPATH += $$_PRO_FILE_PWD_/src/nsrreadercore/3dparty

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

include (config.pri)
