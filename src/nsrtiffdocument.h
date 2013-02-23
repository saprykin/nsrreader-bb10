#ifndef NSRTIFFDOCUMENT_H
#define NSRTIFFDOCUMENT_H

#include "nsrabstractdocument.h"
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
	int getMaxZoom();
	int getMinZoom();

private:
	TIFF			*_tiff;
	bool			_readyForLoad;
	QSize			_cachedPageSize;
	int			_pageCount;
	int			_cachedPage;
	QImage			_origImage;
	QImage			_image;
};

#endif // NSRTIFFDOCUMENT_H
