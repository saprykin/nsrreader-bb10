#include "nsrrenderedpage.h"

NSRRenderedPage::NSRRenderedPage (QObject *parent) :
	QObject (parent),
	_number (0)
{
}

NSRRenderedPage::NSRRenderedPage (int number, QObject *parent) :
	QObject (parent),
	_number (number)
{
}

NSRRenderedPage::NSRRenderedPage (const NSRRenderedPage& page) :
	QObject (page.parent ())
{
	_image = page._image;
	_number = page._number;
}

NSRRenderedPage::~NSRRenderedPage ()
{
}

NSRRenderedPage&
NSRRenderedPage::operator = (const NSRRenderedPage& page)
{
	if (this != &page) {
		_image = page._image;
		_number = page._number;
	}

	return *this;
}

int
NSRRenderedPage::getNumber () const
{
	return _number;
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

void
NSRRenderedPage::setNumber (int number)
{
	_number = number;
}

bool
NSRRenderedPage::isValid () const
{
	return _number > 0 && !_image.isValid ();
}

void
NSRRenderedPage::setImage (bb::ImageData img)
{
	_image = img;
}


