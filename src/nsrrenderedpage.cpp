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
	_text = page._text;
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
		_text = page._text;
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

QString
NSRRenderedPage::getText () const
{
	return _text;
}

bool
NSRRenderedPage::isValid () const
{
	return _number > 0 && _image.isValid ();
}

void
NSRRenderedPage::setNumber (int number)
{
	_number = number;
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



