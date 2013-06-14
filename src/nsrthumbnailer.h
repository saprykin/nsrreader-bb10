#ifndef NSRTHUMBNAILER_H_
#define NSRTHUMBNAILER_H_

#include "nsrrenderedpage.h"

#include <QString>

#include <bb/ImageData>

class NSRThumbnailer
{
public:
	static bool isThumbnailExists (const QString& path);
	static void saveThumbnail (const QString&		path,
				   const NSRRenderedPage&	page,
				   bool				isEncrypted);
	static QString getThumbnailText (const QString& path);
	static QString getThumnailPath (const QString& path);
	static bool isThumbnailEncrypted (const QString& path);
	static void cleanOldFiles ();
	static void setThumbnailEncrypted (const QString&	path,
					   bool			isEncrypted);

private:
	static QString filePathToGroup (const QString& path);

	NSRThumbnailer () {}
	~NSRThumbnailer () {}
};

#endif /* NSRTHUMBNAILER_H_ */
