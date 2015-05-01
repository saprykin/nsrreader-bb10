#ifndef NSRTHUMBNAILER_H_
#define NSRTHUMBNAILER_H_

#include "interfaces/insrthumbnailer.h"
#include "nsrrenderedpage.h"

#include <QString>
#include <QSettings>
#include <QObject>

class NSRThumbnailer : public QSettings, public INSRThumbnailer
{
	Q_OBJECT
public:
	static NSRThumbnailer * instance ();
	static void release ();

	/* INSRThumbnailer interface */
	void saveThumbnail (const QString&		path,
			    const NSRRenderedPage&	page);
	void saveThumbnailEncrypted (const QString& path);
	bool isThumbnailOutdated (const QString& path) const;
	int getThumbnailWidth () const;

	bool isThumbnailExists (const QString& path) const;
	QString getThumbnailText (const QString& path) const;
	QString getThumbnailPath (const QString& path) const;
	bool isThumbnailEncrypted (const QString& path) const;
	void cleanOldFiles ();
	void removeThumbnail (const QString& path);

private:
	QString filePathToHash (const QString& path) const;
	QString getThumbnailPathFromHash (const QString& hash) const;

	NSRThumbnailer ();
	virtual ~NSRThumbnailer () {}

	static NSRThumbnailer *	_instance;
	QString			_configDir;
};

#endif /* NSRTHUMBNAILER_H_ */
