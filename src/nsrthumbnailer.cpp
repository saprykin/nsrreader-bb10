#include "nsrthumbnailer.h"
#include "nsrsettings.h"

#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

#ifdef Q_OS_BLACKBERRY
#  include <bb/utility/ImageConverter>
using namespace bb::utility;
#endif

#define NSR_THUMBNAIL_MAX_TEXT_PRE	600
#define NSR_THUMBNAIL_MAX_TEXT_OUT	500
#define NSR_THUMBNAIL_WIDTH		256

NSRThumbnailer * NSRThumbnailer::_instance = NULL;

NSRThumbnailer::NSRThumbnailer () :
	QSettings (NSRSettings::getSettingsDirectory () + "/thumbnails/thumbnails.ini",
		   QSettings::IniFormat)
{
	_configDir = NSRSettings::getSettingsDirectory () + "/thumbnails";

	QDir dir;

	if (!dir.exists (_configDir))
		dir.mkpath (_configDir);
}

NSRThumbnailer *
NSRThumbnailer::instance ()
{
	if (_instance == NULL)
		_instance = new NSRThumbnailer ();

	return _instance;
}

void
NSRThumbnailer::release ()
{
	if (_instance != NULL) {
		delete _instance;
		_instance = NULL;
	}
}

int
NSRThumbnailer::getThumbnailWidth () const
{
	return NSR_THUMBNAIL_WIDTH;
}

bool
NSRThumbnailer::isThumbnailExists (const QString& path) const
{
	return QFile::exists (getThumbnailPath (path));
}

void
NSRThumbnailer::saveThumbnail (const QString&		path,
			       const NSRRenderedPage&	page)
{
	if (!QFile::exists (path))
		return;

	QString hash = filePathToHash (path);

	if (page.getImage().isValid ()) {
		QString fileName = getThumbnailPathFromHash (hash);

#ifdef Q_OS_BLACKBERRY
		ImageConverter::encode (QUrl::fromLocalFile (fileName),
					page.getImage (),
					50);
#else
		page.getImage().save (fileName);
#endif
	}

	QString pageText = page.getText().left (NSR_THUMBNAIL_MAX_TEXT_PRE);
	int lastIndex = pageText.lastIndexOf (QRegExp ("\\S\\s"));

	/* Do not truncate the whole string */
	if (lastIndex > NSR_THUMBNAIL_MAX_TEXT_OUT)
		pageText = pageText.left (lastIndex + 1) + "...";

	beginGroup (hash);
	setValue ("path", path);
	setValue ("text", pageText);
	setValue ("encrypted", false);
	setValue ("file-size", QFileInfo (path).size ());
	endGroup ();
	sync ();
}

void
NSRThumbnailer::saveThumbnailEncrypted (const QString&	path)
{
	if (!QFile::exists (path))
		return;

	QString hash = filePathToHash (path);

	beginGroup (hash);
	setValue ("path", path);
	setValue ("encrypted", true);
	setValue ("file-size", QFileInfo(path).size ());
	remove ("text");
	endGroup ();
	sync ();

	QFile::remove (getThumbnailPathFromHash (hash));
}

QString
NSRThumbnailer::getThumbnailPath (const QString& path) const
{
	return getThumbnailPathFromHash (filePathToHash (path));
}

void
NSRThumbnailer::cleanOldFiles ()
{
	QStringList files = childGroups ();
	int count = files.count ();

	for (int i = 0; i < count; ++i) {
		beginGroup (files.at (i));

		bool needDelete = false;
		QString filePath =value("path").toString ();

		if (!QFile::exists (filePath)) {
			QFile::remove (getThumbnailPath (filePath));
			needDelete = true;
		}

		endGroup ();

		if (needDelete)
			remove (files.at (i));
	}
}

QString
NSRThumbnailer::getThumbnailText (const QString& path) const
{
	return value(filePathToHash (path) + "/text", "").toString ();
}

bool
NSRThumbnailer::isThumbnailEncrypted (const QString& path) const
{
	return value(filePathToHash (path) + "/encrypted", false).toBool ();
}

void
NSRThumbnailer::removeThumbnail (const QString& path)
{
	QString hash = filePathToHash (path);

	remove (hash);
	QFile::remove (getThumbnailPathFromHash (hash));
}

bool
NSRThumbnailer::isThumbnailOutdated (const QString& path) const
{
	if (!QFile::exists (path))
		return false;

	QFileInfo	fileInfo (path);
	QString		hash = filePathToHash (path);
	QString		filePath = getThumbnailPathFromHash (hash);
	qint64		fileSize = fileInfo.size ();
	qint64		storedFileSize = value(hash + "/file-size", -1).toInt ();

	if (storedFileSize == -1)
		storedFileSize = fileSize;

	return (!QFile::exists (filePath) || fileSize != storedFileSize ||
		fileInfo.lastModified () > QFileInfo(filePath).lastModified ());
}

QString
NSRThumbnailer::filePathToHash (const QString& path) const
{
	return QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();
}

QString
NSRThumbnailer::getThumbnailPathFromHash (const QString& hash) const
{
	return _configDir + "/" + hash + ".png";
}
