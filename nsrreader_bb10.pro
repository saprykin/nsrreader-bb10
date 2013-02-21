APP_NAME = nsrreader_bb10

CONFIG += qt warn_on cascades10

DEFINES += HAVE_CONFIG_H

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

include(config.pri)
