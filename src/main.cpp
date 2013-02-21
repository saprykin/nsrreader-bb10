#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QLocale>
#include <QTranslator>
#include <Qt/qdeclarativedebug.h>
#include "nsrreaderbb10.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main (int argc, char **argv)
{
    Application app (argc, argv);

    QTranslator translator;
    QString locale_string = QLocale().name ();
    QString filename = QString("nsrreader_bb10_%1").arg (locale_string);

    if (translator.load (filename, "app/native/qm")) {
        app.installTranslator (&translator);
    }

    new NSRReaderBB10 (&app);

    return Application::exec ();
}
