#include "nsrimageview.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>

using namespace bb::cascades;

#define NSR_LOGO_WELCOME "asset:///nsrlogo-welcome.png"

NSRImageView::NSRImageView (Container *parent) :
	CustomControl (parent),
	_rootContainer (NULL),
	_imageView (NULL)
{
	_rootContainer = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Fill)
					     .scrollMode (ScrollMode::Both);
	_imageView = ImageView::create().horizontal(HorizontalAlignment::Center)
					.vertical(VerticalAlignment::Center);
	_imageView->setImageSource (QUrl (NSR_LOGO_WELCOME));

	Container *container = Container::create().horizontal(HorizontalAlignment::Fill)
						  .vertical(VerticalAlignment::Fill);
	container->setLayout (DockLayout::create ());
	container->add (_imageView);

	_rootContainer->setContent (container);
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
	_imageView->setImageSource (QUrl (NSR_LOGO_WELCOME));
}
