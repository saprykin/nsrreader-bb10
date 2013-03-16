#ifndef NSRTHUMBNAILER_H_
#define NSRTHUMBNAILER_H_

#include <QString>

#include <bb/ImageData>

class NSRThumbnailer
{
public:
	static bool isThumbnailExists (const QString& path);
	static void saveThumbnail (const QString&	path,
				   bb::ImageData	image);
	static bb::ImageData getThumbnail (const QString& path);
	static QString filePathToThumbnail (const QString& path);

private:
	NSRThumbnailer () {}
	~NSRThumbnailer () {}
};

#endif /* NSRTHUMBNAILER_H_ */
