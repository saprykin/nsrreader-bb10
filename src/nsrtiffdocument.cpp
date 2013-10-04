#include "nsrtiffdocument.h"
#include "nsrpagecropper.h"

#include <math.h>

NSRTIFFDocument::NSRTIFFDocument (const QString& file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_tiff (NULL),
	_readyForLoad (false),
	_pageCount (0),
	_cachedPage (0)
{
	if ((_tiff = TIFFOpen (file.toUtf8().data(), "r")) == NULL)
		return;

	_pageCount = 1;

	while (TIFFReadDirectory (_tiff))
		++_pageCount;

	if ((_pageCount > 0 && TIFFSetDirectory (_tiff, 0) != 0) || _pageCount == 0) {
		uint32 w = 0, h = 0;

		TIFFGetField (_tiff, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField (_tiff, TIFFTAG_IMAGELENGTH, &h);

		_cachedPageSize = QSize (w, h);
	}
}

NSRTIFFDocument::~NSRTIFFDocument ()
{
	if (_tiff != NULL) {
		TIFFClose (_tiff);
		_tiff = NULL;
	}

	if (!_origImage.isNull ())
		_origImage = QImage ();

	if (!_image.isNull ())
		_image = QImage ();
}


int
NSRTIFFDocument::getNumberOfPages () const
{
	if (_tiff == NULL)
		return 0;

	return _pageCount > 0 ? _pageCount : 1;
}

bool
NSRTIFFDocument::isValid () const
{
	return (_tiff != NULL);
}

void
NSRTIFFDocument::renderPage (int page)
{
	uint32	w = 0, h = 0;
	size_t	npixels;
	char	*imgBuf;
	QImage	*img;

	if (_tiff == NULL || page > getNumberOfPages () || page < 1)
		return;

	if (_pageCount > 0 && TIFFSetDirectory (_tiff, page - 1) == 0)
		return;

	TIFFGetField (_tiff, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField (_tiff, TIFFTAG_IMAGELENGTH, &h);
	npixels = w * h;

	_readyForLoad = false;

	if (npixels * sizeof (uint32) > NSR_DOCUMENT_MAX_HEAP)
		return;

	if (!_image.isNull ())
		_image = QImage ();

	double pageWidth = (getRotation () == 90 || getRotation () == 270) ? h : w;

	if (isZoomToWidth ()) {
		double wZoom = ((double) getScreenWidth () / pageWidth * 100.0);
		setZoomSilent (wZoom);
	}

	if (getZoom () < getMinZoom ())
		setZoomSilent (getMinZoom ());

	if (_cachedPage == page && !_origImage.isNull ()) {
		double scale = getZoom () / 100.0;
		QTransform trans;

		trans.scale (scale, scale);
		trans.rotate (getRotation ());
		_image = _origImage.transformed (trans);

		if (isAutoCrop ())
			updateCropPads ();

		_readyForLoad = true;
		return;
	}

	if (!_origImage.isNull ())
		_origImage = QImage ();

	imgBuf = new char[npixels * sizeof (uint32)];
	img = new QImage ((const uchar*) imgBuf, w, h, w * sizeof (uint32), QImage::Format_ARGB32);

	if (TIFFReadRGBAImageOriented (_tiff, w, h, (uint32 *) img->bits (), ORIENTATION_TOPLEFT, 0) == 0) {
		delete img;
		delete imgBuf;
	} else {
		uint32 orientationTag = 0;

		if (TIFFGetField (_tiff, TIFFTAG_ORIENTATION, &orientationTag) == 0) {
			switch (orientationTag) {
			case ORIENTATION_LEFTTOP:
				rotateRightMirrorHorizontal (&img, &imgBuf);
				break;
			case ORIENTATION_RIGHTTOP:
				rotateRightMirrorVertical (&img, &imgBuf);
				break;
			case ORIENTATION_RIGHTBOT:
				rotateRightMirrorHorizontal (&img, &imgBuf);
				break;
			case ORIENTATION_LEFTBOT:
				rotateRightMirrorVertical (&img, &imgBuf);
				break;
			default:
				break;
			}
		} else {
			/* Convert from ABGR to ARGB pixel format */
			uint32 *dataPtr = reinterpret_cast<uint32 *> (img->bits ());

			for (uint32 row = 0; row < h; ++row)
				for (uint32 col = 0; col < w; ++col) {
					uint32 pxl  = *(dataPtr + row * w + col);
					*(dataPtr + row * w + col) = ((pxl & 0x000000FF) << 16) |
							(pxl & 0xFF000000) |
							((pxl & 0x00FF0000) >> 16) |
							(pxl & 0x0000FF00);
				}
		}

		double scale = getZoom () / 100.0;
		QTransform trans;

		trans.scale (scale, scale);
		trans.rotate (getRotation ());

		if (isAutoCrop ()) {
			_pads = NSRPageCropper::findCropPads ((unsigned char *) img->bits (),
							      NSRPageCropper::NSR_PIXEL_ORDER_ARGB,
							      img->width (), img->height (), img->bytesPerLine ());
			updateCropPads ();
		} else
			_pads = NSRCropPads ();

		if (_origImage.byteCount () > NSR_DOCUMENT_MAX_HEAP / (2 + scale * scale)) {
			_image = img->transformed (trans);
			_cachedPage = 0;

			delete img;
			delete imgBuf;
		} else {
			_origImage = img->copy ();

			delete img;
			delete imgBuf;

			_image = _origImage.transformed (trans);
			_cachedPage = page;
		}

		_readyForLoad = true;
	}
}

double
NSRTIFFDocument::getMaxZoom ()
{
	if (_tiff == NULL)
		return 0;

	if (_cachedPageSize == QSize (0, 0))
		return 100;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = _cachedPageSize.width () * _cachedPageSize.height () * 4;

	return validateMaxZoom (_cachedPageSize, sqrt (NSR_DOCUMENT_MAX_HEAP / pageSize - 1) * 100 + 0.5);
}

double
NSRTIFFDocument::getMinZoom ()
{
	if (_cachedPageSize == QSize (0, 0))
		return 25;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = _cachedPageSize.width () * _cachedPageSize.height () * 4;

	if (pageSize > NSR_DOCUMENT_MAX_HEAP)
		return getMaxZoom ();
	else
		return (getMaxZoom () / 10) > 25 ? 25 : getMaxZoom () / 10;
}

bb::ImageData
NSRTIFFDocument::getCurrentPage ()
{
	if (!_readyForLoad)
		return bb::ImageData ();

	if (_image.isNull())
		return bb::ImageData ();

	bb::ImageData imgData (bb::PixelFormat::RGBA_Premultiplied,
			       _image.width () - _pads.getLeft () - _pads.getRight (),
			       _image.height () - _pads.getTop () - _pads.getBottom ());

	unsigned char *addr = (unsigned char *) imgData.pixels ();
	int stride = imgData.bytesPerLine ();
	int bw = _image.width ();
	int bh = _image.height ();
	int rowBytes = _image.bytesPerLine ();
	unsigned char *dataPtr = _image.bits ();

	for (int i = _pads.getTop (); i < bh - _pads.getBottom (); ++i) {
		unsigned char *inAddr = (unsigned char *) (dataPtr + i * rowBytes);

		for (int j = _pads.getLeft (); j < bw - _pads.getRight (); ++j) {
			if (isInvertedColors ()) {
				addr[(j - _pads.getLeft ()) * 4 + 0] = 255 - inAddr[j * 4 + 2];
				addr[(j - _pads.getLeft ()) * 4 + 1] = 255 - inAddr[j * 4 + 1];
				addr[(j - _pads.getLeft ()) * 4 + 2] = 255 - inAddr[j * 4 + 0];
				addr[(j - _pads.getLeft ()) * 4 + 3] = 255;
			} else {
				addr[(j - _pads.getLeft ()) * 4 + 0] = inAddr[j * 4 + 2];
				addr[(j - _pads.getLeft ()) * 4 + 1] = inAddr[j * 4 + 1];
				addr[(j - _pads.getLeft ()) * 4 + 2] = inAddr[j * 4 + 0];
				addr[(j - _pads.getLeft ()) * 4 + 3] = 255;
			}
		}

		addr += stride;
	}

	return imgData;
}

void
NSRTIFFDocument::updateCropPads ()
{
	double scale = getZoom () / 100.0;

	_pads.setScale (scale);
	_pads.setRotation (getRotation ());
}

/* The following code was taken from the Qt TIFF handler plugin with some modifications */
void
NSRTIFFDocument::rotateRightMirrorHorizontal (QImage** const image, char **buf)
{
	const int height = (*image)->height ();
	const int width = (*image)->width ();

	char *newBuf = new char[height * width * sizeof (uint32)];
	QImage *generated = new QImage ((const uchar *) newBuf, height, width, height * sizeof (uint32), (*image)->format ());
	const uint32 *pixel = reinterpret_cast<const uint32 *> ((*image)->bits ());
	uint32 *const generatedBits = reinterpret_cast<uint32 *> (generated->bits ());

	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			int idx = col * height + row;
			/* Also convert from ABGR to ARGB */
			generatedBits[idx] = ((*pixel & 0x000000FF) << 16) |
					     (*pixel & 0xFF000000) |
					     ((*pixel & 0x00FF0000) >> 16) |
					     (*pixel & 0x0000FF00);
			++pixel;
		}
	}

	delete *image;
	delete *buf;

	*image = generated;
	*buf = newBuf;
}

void
NSRTIFFDocument::rotateRightMirrorVertical (QImage** const image, char **buf)
{
	const int height = (*image)->height ();
	const int width = (*image)->width ();

	char *newBuf = new char[height * width * sizeof (uint32)];
	QImage *generated = new QImage ((const uchar *) newBuf, height, width, height * sizeof (uint32), (*image)->format ());
	const int lastCol = width - 1;
	const int lastRow = height - 1;

	const uint32 *pixel = reinterpret_cast<const uint32 *> ((*image)->bits ());
	uint32 *const generatedBits = reinterpret_cast<uint32 *> (generated->bits ());

	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			int idx = (lastCol - col) * height + (lastRow - row);
			/* Also convert from ABGR to ARGB */
			generatedBits[idx] = ((*pixel & 0x000000FF) << 16) |
					     (*pixel & 0xFF000000) |
					     ((*pixel & 0x00FF0000) >> 16) |
					     (*pixel & 0x0000FF00);
			++pixel;
		}
	}

	delete *image;
	delete *buf;

	*image = generated;
	*buf = newBuf;
}
