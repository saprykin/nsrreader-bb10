#include "nsrimageview.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>

using namespace bb::cascades;

#define NSR_LOGO_WELCOME "asset:///nsrlogo-welcome.png"

NSRImageView::NSRImageView (Container *parent) :
	CustomControl (parent),
	_scrollView (NULL),
	_imageView (NULL),
	_textArea (NULL),
	_rootContainer (NULL),
	_viewMode (NSR_VIEW_MODE_GRAPHIC)
{
	_scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Fill)
					  .scrollMode (ScrollMode::Both);
	_imageView = ImageView::create().horizontal(HorizontalAlignment::Center)
					.vertical(VerticalAlignment::Center);
	_imageView->setImageSource (QUrl (NSR_LOGO_WELCOME));

	Container *container = Container::create().horizontal(HorizontalAlignment::Fill)
						  .vertical(VerticalAlignment::Fill)
						  .layout(DockLayout::create ());
	container->add (_imageView);
	_scrollView->setContent (container);

	_textArea = TextArea::create().horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Fill)
				      .editable(false)
				      .visible(false)
				      .inputFlags(TextInputFlag::SpellCheckOff |
						  TextInputFlag::PredictionOff |
						  TextInputFlag::AutoCapitalizationOff |
						  TextInputFlag::AutoCorrectionOff |
						  TextInputFlag::AutoPeriodOff |
						  TextInputFlag::WordSubstitutionOff |
						  TextInputFlag::VirtualKeyboardOff);

	_rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill);
	_rootContainer->add (_scrollView);
	_rootContainer->add (_textArea);

	setRoot (_rootContainer);
}

NSRImageView::~NSRImageView ()
{
}

void
NSRImageView::setPage (const NSRRenderedPage& page)
{
	_imageView->setImage (page.getImage ());
	_textArea->setText (page.getText ());
}

void
NSRImageView::resetPage ()
{
	_imageView->setImageSource (QUrl (NSR_LOGO_WELCOME));
	_textArea->resetText ();
}

void
NSRImageView::setViewMode (NSRImageView::NSRViewMode mode)
{
	if (_viewMode == mode)
		return;

	switch (mode) {
	case NSR_VIEW_MODE_GRAPHIC:
		_textArea->setVisible (false);
		_scrollView->setVisible (true);
		_rootContainer->setBackground (Color::Black);
		_viewMode = mode;
		break;
	case NSR_VIEW_MODE_TEXT:
		_textArea->setVisible (true);
		_scrollView->setVisible (false);
		_rootContainer->setBackground (Color::White);
		_viewMode = mode;
		break;
	default:
		return;
	}
}

NSRImageView::NSRViewMode
NSRImageView::getViewMode () const
{
	return _viewMode;
}

void
NSRImageView::setScrollPosition (const QPointF& pos)
{
	_scrollView->scrollToPoint (pos.x (), pos.y ());
}

QPointF
NSRImageView::getScrollPosition () const
{
	return _scrollView->viewableArea().topLeft ();
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
