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
	return QFile::exists (filePathToThumbnail (path));
}

void
NSRThumbnailer::saveThumbnail (const QString&	path,
                	       bb::ImageData	image)
{
	QDir dir;
	QSettings settings (NSR_THUMBNAILS_DIR + "/thumbnails.ini",
			    QSettings::IniFormat);

	if (!dir.exists (NSR_THUMBNAILS_DIR))
		dir.mkpath (NSR_THUMBNAILS_DIR);

	QString fileName = filePathToThumbnail (path);

	ImageConverter::encode (QUrl::fromLocalFile (fileName),
				image,
				50);

	settings.beginGroup ("Thumbnails");
	settings.setValue (path, QFileInfo (fileName).fileName ());
	settings.endGroup ();
	settings.sync ();
}

bb::ImageData
NSRThumbnailer::getThumbnail (const QString& path)
{
	if (isThumbnailExists (path))
		return ImageConverter::decode (QUrl::fromLocalFile (filePathToThumbnail (path)));
	else
		return bb::ImageData ();
}

QString
NSRThumbnailer::filePathToThumbnail (const QString& path)
{
	QString hash = QCryptographicHash::hash(path.toAscii (), QCryptographicHash::Md5).toHex ();

	return NSR_THUMBNAILS_DIR + "/" + hash + ".png";
}
