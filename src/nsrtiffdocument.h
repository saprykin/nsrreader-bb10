#ifndef NSRTIFFDOCUMENT_H
#define NSRTIFFDOCUMENT_H

#include "nsrabstractdocument.h"
#include "nsrcroppads.h"
#include "tiff/tiffio.h"

class NSRTIFFDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRTIFFDocument(const QString& file, QObject *parent = 0);
	~NSRTIFFDocument();
	int getNumberOfPages() const;
	void renderPage(int page);
	bb::ImageData getCurrentPage ();
	bool isValid() const;
	double getMaxZoom();
	double getMinZoom();

private:
	void updateCropPads ();
	void rotateRightMirrorHorizontal (QImage ** image, char **buf);
	void rotateRightMirrorVertical (QImage ** image, char **buf);

	NSRCropPads		_pads;
	TIFF			*_tiff;
	bool			_readyForLoad;
	QSize			_cachedPageSize;
	int			_pageCount;
	int			_cachedPage;
	QImage			_origImage;
	QImage			_image;
};

#endif // NSRTIFFDOCUMENT_H
