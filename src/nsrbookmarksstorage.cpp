#include "nsrbookmarksstorage.h"
#include "nsrsettings.h"

#include <QCryptographicHash>
#include <QFile>

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

	beginGroup (filePathToHash (file));
	setValue ("bookmarks", bookmarks);
	setValue ("file", file);
	endGroup ();

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

void
NSRBookmarksStorage::cleanOldFiles ()
{
	QStringList childs = childGroups ();
	int count = childs.count ();

	for (int i = 0; i < count; ++i)
		if (!QFile::exists (value(childs.at (i) + "/file", "").toString ()))
			remove (childs.at(i));

	sync ();
}

QString
NSRBookmarksStorage::filePathToHash (const QString& path)
{
	return QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();
}
