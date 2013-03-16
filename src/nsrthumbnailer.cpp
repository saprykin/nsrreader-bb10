#include "nsrthumbnailer.h"
#include "nsrsettings.h"

#include <QSettings>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>

#include <bb/utility/ImageConverter>

using namespace bb::utility;

#define NSR_THUMBNAILS_DIR NSRSettings::getSettingsDirectory () + "/thumbnails"

bool
NSRThumbnailer::isThumbnailExists (const QString& path)
{
	return QFile::exists (getThumnailPath (path));
}

void
NSRThumbnailer::saveThumbnail (const QString&		path,
			       const NSRRenderedPage&	page)
{
	QDir dir;
	QSettings settings (NSR_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	if (!dir.exists (NSR_THUMBNAILS_DIR))
		dir.mkpath (NSR_THUMBNAILS_DIR);

	if (page.getImage().isValid ()) {
		QString fileName = getThumnailPath (path);

		ImageConverter::encode (QUrl::fromLocalFile (fileName),
					page.getImage (),
					50);
	}

	settings.beginGroup (filePathToGroup (path));
	settings.setValue ("path", path);
	settings.setValue ("text", page.getText ());
	settings.endGroup ();
	settings.sync ();
}

QString
NSRThumbnailer::getThumnailPath (const QString& path)
{
	QString hash = QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();

	return NSR_THUMBNAILS_DIR + "/" + hash + ".png";
}

void
NSRThumbnailer::cleanOldFiles ()
{
	QSettings settings (NSR_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	QStringList files = settings.childGroups ();
	int count = files.count ();

	for (int i = 0; i < count; ++i) {
		settings.beginGroup (files.at (i));

		bool needDelete = false;
		QString filePath = settings.value("path").toString ();

		if (!QFile::exists (filePath)) {
			QFile::remove (getThumnailPath (filePath));
			needDelete = true;
		}

		settings.endGroup ();

		if (needDelete)
			settings.remove (files.at (i));
	}

	settings.endGroup ();
}

QString
NSRThumbnailer::getThumbnailText (const QString& path)
{
	QSettings settings (NSR_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	return settings.value(filePathToGroup (path) + "/text", "").toString ();
}

QString
NSRThumbnailer::filePathToGroup (const QString& path)
{
	return QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();
}


