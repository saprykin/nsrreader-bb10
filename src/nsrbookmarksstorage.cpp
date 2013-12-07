#include "nsrbookmarksstorage.h"
#include "nsrsettings.h"

#include <QCryptographicHash>

NSRBookmarksStorage * NSRBookmarksStorage::_instance = NULL;

NSRBookmarksStorage::NSRBookmarksStorage () :
		QSettings (NSRSettings::getSettingsDirectory () + "/bookmarks.ini",
			   QSettings::IniFormat)
{
}

NSRBookmarksStorage::~NSRBookmarksStorage ()
{
}

NSRBookmarksStorage *
NSRBookmarksStorage::instance ()
{
	if (_instance == NULL)
		_instance = new NSRBookmarksStorage ();

	return _instance;
}

void
NSRBookmarksStorage::release ()
{
	if (_instance != NULL) {
		delete _instance;
		_instance = NULL;
	}
}

QString
NSRBookmarksStorage::getBookmarks (const QString& file)
{
	if (file.isEmpty ())
		return QString ();

	QString res = value(filePathToHash (file) + "/bookmarks", QString ()).toString ();

	return res;
}

void
NSRBookmarksStorage::saveBookmarks (const QString& file,
				    const QString& bookmarks)
{
	if (file.isEmpty ())
		return;

	setValue (filePathToHash (file) + "/bookmarks", bookmarks);
	sync ();
}

void
NSRBookmarksStorage::removeBookmarks (const QString& file)
{
	if (file.isEmpty ())
		return;

	remove (filePathToHash (file));
	sync ();
}

QString
NSRBookmarksStorage::filePathToHash (const QString& path)
{
	return QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();
}

