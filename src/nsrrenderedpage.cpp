#include "nsrrenderedpage.h"

NSRRenderedPage::NSRRenderedPage (QObject *parent) :
	QObject (parent),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0),
	_number (0),
	_cropped (false)
{
}

NSRRenderedPage::NSRRenderedPage (int number, QObject *parent) :
	QObject (parent),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0),
	_number (number),
	_cropped (false)
{
}

NSRRenderedPage::NSRRenderedPage (const NSRRenderedPage& page) :
	QObject (page.parent ())
{
	_reason		= page._reason;
	_image		= page._image;
	_zoom		= page._zoom;
	_number		= page._number;
	_text		= page._text;
	_lastPos	= page._lastPos;
	_lastTextPos	= page._lastTextPos;
	_cropped	= page._cropped;
}

NSRRenderedPage::~NSRRenderedPage ()
{
}

NSRRenderedPage&
NSRRenderedPage::operator = (const NSRRenderedPage& page)
{
	if (this != &page) {
		_reason		= page._reason;
		_image		= page._image;
		_zoom		= page._zoom;
		_number		= page._number;
		_text		= page._text;
		_lastPos	= page._lastPos;
		_lastTextPos	= page._lastTextPos;
		_cropped	= page._cropped;
	}

	return *this;
}

NSRRenderedPage::NSRRenderReason
NSRRenderedPage::getRenderReason () const
{
	return _reason;
}

int
NSRRenderedPage::getNumber () const
{
	return _number;
}

double
NSRRenderedPage::getZoom () const
{
	return _zoom;
}

QSize
NSRRenderedPage::getSize () const
{
	return QSize (_image.width (), _image.height ());
}

bb::ImageData
NSRRenderedPage::getImage () const
{
	return _image;
}

QString
NSRRenderedPage::getText () const
{
	return _text;
}

QPointF
NSRRenderedPage::getLastPosition () const
{
	return _lastPos;
}

QPointF
NSRRenderedPage::getLastTextPosition () const
{
	return _lastTextPos;
}

bool
NSRRenderedPage::isValid () const
{
	return _number > 0;
}

bool
NSRRenderedPage::isImageValid () const
{
	return _number > 0 && _image.isValid ();
}

bool

NSRRenderedPage::isEmpty () const
{
	return _text.isEmpty () && !_image.isValid ();
}

bool
NSRRenderedPage::isCropped () const
{
	return _cropped;
}

void
NSRRenderedPage::setRenderReason (NSRRenderedPage::NSRRenderReason reason)
{
	_reason = reason;
}

void
NSRRenderedPage::setNumber (int number)
{
	_number = number;
}

void
NSRRenderedPage::setZoom (double zoom)
{
	if (zoom < 0)
		zoom = 0;

	_zoom = zoom;
}

void
NSRRenderedPage::setImage (bb::ImageData img)
{
	_image = img;
}

void
NSRRenderedPage::setText (const QString& text)
{
	_text = text;
}

void
NSRRenderedPage::setLastPosition (const QPointF& pos)
{
	_lastPos = pos;
}

void
NSRRenderedPage::setLastTextPosition (const QPointF& pos)
{
	_lastTextPos = pos;
}

void
NSRRenderedPage::setCropped (bool cropped)
{
	_cropped = cropped;
}
