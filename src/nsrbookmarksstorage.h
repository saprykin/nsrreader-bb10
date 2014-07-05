#ifndef NSRBOOKMARKSSTORAGE_H_
#define NSRBOOKMARKSSTORAGE_H_

#include <QSettings>

class NSRBookmarksStorage : public QSettings
{
	Q_OBJECT
public:
	static NSRBookmarksStorage * instance ();
	static void release ();

	QString getBookmarks (const QString& file);
	void saveBookmarks (const QString& file, const QString& bookmarks);
	void removeBookmarks (const QString& file);
	void cleanOldFiles ();

private:
	QString filePathToHash (const QString& path);

	NSRBookmarksStorage ();
	~NSRBookmarksStorage ();

	static NSRBookmarksStorage *	_instance;
};

#endif /* NSRBOOKMARKSSTORAGE_H_ */
