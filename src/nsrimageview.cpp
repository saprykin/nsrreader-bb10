#include "nsrimageview.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>

using namespace bb::cascades;

NSRImageView::NSRImageView (Container *parent) :
	CustomControl (parent),
	_rootContainer (NULL),
	_imageView (NULL)
{
	_rootContainer = Container::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Center);
	_rootContainer->setLayout (DockLayout::create ());
	_imageView = ImageView::create().horizontal(HorizontalAlignment::Center)
					.vertical(VerticalAlignment::Center);
	_rootContainer->setBackground (Color::Black);
	_rootContainer->add (_imageView);
	setRoot (_rootContainer);
}

NSRImageView::~NSRImageView ()
{
}

void
NSRImageView::onHeightChanged (float height)
{
	_rootContainer->setPreferredHeight (height);
}

void
NSRImageView::onWidthChanged (float width)
{
	_rootContainer->setPreferredWidth (width);
}

Image
NSRImageView::getImage () const
{
	return _imageView->image ();
}

void
NSRImageView::setImage (const Image &img)
{
	_imageView->setImage (img);
}

void
NSRImageView::clearImage ()
{
	_imageView->resetImage ();
}
