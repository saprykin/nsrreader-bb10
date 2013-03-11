#include "nsrpageview.h"
#include "nsrsettings.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/TapHandler>

using namespace bb::cascades;

#define NSR_LOGO_WELCOME "asset:///nsrlogo-welcome.png"

NSRPageView::NSRPageView (Container *parent) :
	CustomControl (parent),
	_scrollView (NULL),
	_imageView (NULL),
	_textArea (NULL),
	_rootContainer (NULL),
	_textContainer (NULL),
	_viewMode (NSR_VIEW_MODE_GRAPHIC),
	_isInvertedColors (false)
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

	_textContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(DockLayout::create())
					    .background(Color::White)
					    .visible(false);
	_textArea = TextArea::create().horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Fill)
				      .editable(false)
				      .inputFlags(TextInputFlag::SpellCheckOff |
						  TextInputFlag::PredictionOff |
						  TextInputFlag::AutoCapitalizationOff |
						  TextInputFlag::AutoCorrectionOff |
						  TextInputFlag::AutoPeriodOff |
						  TextInputFlag::WordSubstitutionOff |
						  TextInputFlag::VirtualKeyboardOff);
	_textContainer->add (_textArea);

	_rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(DockLayout::create())
					    .background(Color::Black);
	_rootContainer->add (_scrollView);
	_rootContainer->add (_textContainer);

	TapHandler *imgTapHandler = TapHandler::create().onTapped (this, SLOT (onTappedGesture (bb::cascades::TapEvent *)));
	TapHandler *txtTapHandler = TapHandler::create().onTapped (this, SLOT (onTappedGesture (bb::cascades::TapEvent *)));
	_scrollView->addGestureHandler (imgTapHandler);
	_textContainer->addGestureHandler (txtTapHandler);

	setRoot (_rootContainer);

	setInvertedColors (NSRSettings().isInvertedColors ());
}

NSRPageView::~NSRPageView ()
{
}

void
NSRPageView::setPage (const NSRRenderedPage& page)
{
	_imageView->setImage (page.getImage ());
	_textArea->setText (page.getText ());
}

void
NSRPageView::resetPage ()
{
	_imageView->setImageSource (QUrl (NSR_LOGO_WELCOME));
	_textArea->resetText ();
}

void
NSRPageView::setViewMode (NSRPageView::NSRViewMode mode)
{
	if (_viewMode == mode)
		return;

	switch (mode) {
	case NSR_VIEW_MODE_GRAPHIC:
		_textContainer->setVisible (false);
		_scrollView->setVisible (true);
		_viewMode = mode;
		break;
	case NSR_VIEW_MODE_TEXT:
		_textContainer->setVisible (true);
		_scrollView->setVisible (false);
		_viewMode = mode;
		break;
	default:
		return;
	}
}

bool
NSRPageView::isInvertedColors () const
{
	return _isInvertedColors;
}

void
NSRPageView::setInvertedColors (bool inv)
{
	if (_isInvertedColors == inv)
		return;

	_isInvertedColors = inv;

	if (_isInvertedColors) {
		_textArea->textStyle()->setColor (Color::White);
		_textContainer->setBackground (Color::Black);
	} else {
		_textArea->textStyle()->setColor (Color::Black);
		_textContainer->setBackground (Color::White);
	}
}

NSRPageView::NSRViewMode
NSRPageView::getViewMode () const
{
	return _viewMode;
}

void
NSRPageView::setScrollPosition (const QPointF& pos)
{
	_scrollView->scrollToPoint (pos.x (), pos.y ());
}

QPointF
NSRPageView::getScrollPosition () const
{
	return _scrollView->viewableArea().topLeft ();
}

void
NSRPageView::onHeightChanged (float height)
{
	_rootContainer->setPreferredHeight (height);
}

void
NSRPageView::onWidthChanged (float width)
{
	_rootContainer->setPreferredWidth (width);
}

void
NSRPageView::onTappedGesture (bb::cascades::TapEvent *ev)
{
	ev->accept ();
	emit viewTapped ();
}
