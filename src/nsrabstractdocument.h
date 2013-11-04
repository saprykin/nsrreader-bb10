#ifndef NSRABSTRACTDOCUMENT_H
#define NSRABSTRACTDOCUMENT_H

#include <QObject>
#include <QtGui/QPixmap>

#include <bb/ImageData>
#include <bb/PixelFormat>

#define NSR_DOCUMENT_MAX_HEAP	(0x6000000 * 0.50)

class NSRAbstractDocument : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRDocumentError)
	Q_ENUMS (NSRDocumentStyle)
public:
	enum NSRDocumentError {
		NSR_DOCUMENT_ERROR_NO		= 0,
		NSR_DOCUMENT_ERROR_PASSWD	= 1,
		NSR_DOCUMENT_ERROR_TOO_LARGE	= 2,
		NSR_DOCUMENT_ERROR_UNKNOWN	= 3
	};
	enum NSRDocumentStyle {
		NSR_DOCUMENT_STYLE_GRAPHIC	= 1,
		NSR_DOCUMENT_STYLE_TEXT		= 2
	};

	explicit NSRAbstractDocument(const QString& file, QObject *parent = 0);
	virtual ~NSRAbstractDocument ();
	inline QString getDocumentPath () const {return _docPath;}
	virtual int getNumberOfPages () const		= 0;
	virtual void renderPage (int page)		= 0;
	virtual bb::ImageData getCurrentPage ()		= 0;
	virtual bool isValid ()	const			= 0;
	double getZoom () const {return _zoom;}
	void setZoom (double zoom);
        void setZoomSilent (double zoom) {_zoom = zoom;}
	virtual double getMaxZoom ()			= 0;
	virtual double getMinZoom ()			= 0;
	virtual void zoomToWidth (int screenWidth);
	bool isZoomToWidth () const {return _zoomToWidth;}
	int getScreenWidth () const {return _screenWidth;}
	virtual void rotateLeft ();
	virtual void rotateRight ();
	virtual void setRotation (int angle);
	virtual int getRotation () const {return _rotation;}
	virtual QString getText () {return trUtf8 ("No text data available for this page");}
	virtual void setTextOnly (bool textOnly) {_textOnly = textOnly;}
	virtual bool isTextOnly () const {return _textOnly;}
	void setInvertedColors (bool isInverted) {_invertedColors = isInverted;}
	bool isInvertedColors () const {return _invertedColors;}
	void setAutoCrop (bool isAutoCrop) {_autoCrop = isAutoCrop;}
	bool isAutoCrop () const {return _autoCrop;}
	NSRDocumentError getLastError () const {return _lastError;}
	virtual void setPassword (const QString& passwd) {_password = passwd;}
	virtual QString getPassword () const {return _password;}
	virtual void setEncoding (const QString& enc);
	virtual QString getEncoding () const {return _encoding;}
	virtual bool isEncodingUsed () const {return false;}
	void setMaximumPageSize (const QSize& size) {_maxPageSize = size;}
	QSize getMaximumPageSize () const {return _maxPageSize;}
	virtual bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const = 0;
	virtual NSRAbstractDocument::NSRDocumentStyle getPrefferedDocumentStyle () const = 0;

protected:
	void setZoomToWidth (bool toWidth) {_zoomToWidth = toWidth;}
	void setLastError (NSRDocumentError err) {_lastError = err;}
	QString processText(const QString& text);
	double validateMaxZoom (const QSize& pageSize, double zoom) const;

private:
	QString		_docPath;
	QString		_password;
	double		_zoom;
	int		_screenWidth;
	bool		_zoomToWidth;
	bool		_textOnly;
	bool		_invertedColors;
	bool		_autoCrop;
	NSRDocumentError	_lastError;
	QString		_encoding;
	int		_rotation;
	QSize		_maxPageSize;
};

#endif // NSRABSTRACTDOCUMENT_H
