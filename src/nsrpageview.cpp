#include "nsrpageview.h"
#include "nsrsettings.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/Color>
#include <bb/cascades/TapHandler>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/PinchHandler>
#include <bb/cascades/ImageTracker>

using namespace bb::cascades;

#define NSR_LOGO_WELCOME "asset:///nsrlogo-welcome.png"

NSRPageView::NSRPageView (Container *parent) :
	Container (parent),
	_scrollView (NULL),
	_imageView (NULL),
	_textArea (NULL),
	_rootContainer (NULL),
	_textContainer (NULL),
	_viewMode (NSR_VIEW_MODE_GRAPHIC),
	_currentZoom (0),
	_minZoom (0),
	_maxZoom (0),
	_isInvertedColors (false),
	_useDelayedScroll (false)
{
	_scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Fill)
					  .scrollMode(ScrollMode::Both);
	_imageView = ImageView::create().horizontal(HorizontalAlignment::Center)
					.vertical(VerticalAlignment::Center);
	_imageView->setImageSource (QUrl (NSR_LOGO_WELCOME));

	Container *container = Container::create().horizontal(HorizontalAlignment::Fill)
						  .vertical(VerticalAlignment::Fill)
						  .layout(DockLayout::create ())
						  .background(Color::Black);
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
	_textArea->textStyle()->setColor (Color::Black);
	_textContainer->add (_textArea);

	LayoutUpdateHandler::create(this).onLayoutFrameChanged (this,
							       SLOT (onLayoutFrameChanged (QRectF)));

	TapHandler *imgTapHandler = TapHandler::create().onTapped (this, SLOT (onTappedGesture (bb::cascades::TapEvent *)));
	TapHandler *txtTapHandler = TapHandler::create().onTapped (this, SLOT (onTappedGesture (bb::cascades::TapEvent *)));
	_scrollView->addGestureHandler (imgTapHandler);
	_textContainer->addGestureHandler (txtTapHandler);

	PinchHandler *pinchHandler = PinchHandler::create().onPinch (this,
						   SLOT (onPinchStarted (bb::cascades::PinchEvent *)),
						   SLOT (onPinchUpdated (bb::cascades::PinchEvent *)),
						   SLOT (onPinchEnded (bb::cascades::PinchEvent *)),
						   SLOT (onPinchCancelled ()));
	_scrollView->addGestureHandler (pinchHandler);

	add (_scrollView);
	add (_textContainer);

	setInvertedColors (NSRSettings().isInvertedColors ());
}

NSRPageView::~NSRPageView ()
{
}

void
NSRPageView::setPage (const NSRRenderedPage& page)
{
	_textArea->setText (page.getText ());
	_imageView->setImage (page.getImage ());
	_imageView->setPreferredSize (page.getSize().width (), page.getSize().height ());
	_currentZoom = page.getZoom ();

	if (_useDelayedScroll) {
		_useDelayedScroll = false;
		setScrollPosition (_delayedScrollPos);
	} else if (page.getRenderReason () == NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION)
		setScrollPosition (QPointF (0.0, 0.0));
}

void
NSRPageView::resetPage ()
{
	ImageTracker tracker (QUrl (NSR_LOGO_WELCOME));

	_imageView->setImageSource (tracker.imageSource ());
	_imageView->setPreferredSize (tracker.width (), tracker.height ());
	_textArea->resetText ();
	_currentZoom = 100;
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

QSize
NSRPageView::getSize () const
{
	return _size;
}

void
NSRPageView::setZoomRange (int minZoom, int maxZoom)
{
	if (minZoom < 0)
		minZoom = 0;

	if (maxZoom < minZoom)
		maxZoom = minZoom;

	_minZoom = minZoom;
	_maxZoom = maxZoom;
}

void
NSRPageView::fitToWidth () {
	if (_viewMode != NSR_VIEW_MODE_GRAPHIC)
		return;

	if (_imageView->image().isNull ())
		return;

	double scale = _size.width () / _imageView->preferredWidth ();

	_imageView->setPreferredSize (_imageView->preferredWidth () * scale,
				      _imageView->preferredHeight () * scale);
	_currentZoom *= scale;

	emit zoomChanged (_currentZoom, true);
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
NSRPageView::setScrollPositionOnLoad (const QPointF& pos)
{
	_useDelayedScroll = true;
	_delayedScrollPos = pos;
}

void
NSRPageView::onTappedGesture (bb::cascades::TapEvent *ev)
{
	ev->accept ();
	emit viewTapped ();
}

void
NSRPageView::onLayoutFrameChanged (const QRectF& rect)
{
	_size = QSize ((int) rect.width (),
		       (int) rect.height ());
}

void
NSRPageView::onPinchStarted (bb::cascades::PinchEvent* event)
{
	_initialScaleSize = QSize (_imageView->preferredWidth(),
				   _imageView->preferredHeight());

	event->accept ();
}

void
NSRPageView::onPinchUpdated (bb::cascades::PinchEvent* event)
{
	double scale = event->pinchRatio ();

	if (scale * _currentZoom < _minZoom)
		scale = ((double) _minZoom) / _currentZoom;
	else if (scale * _currentZoom > _maxZoom)
		scale = (double) _maxZoom / _currentZoom;

	_imageView->setPreferredSize (_initialScaleSize.width () * scale,
				      _initialScaleSize.height () * scale);

	event->accept ();
}

void
NSRPageView::onPinchEnded (bb::cascades::PinchEvent* event)
{
	double scale = _imageView->preferredWidth () / _initialScaleSize.width ();

	_currentZoom *= scale;

	emit zoomChanged (_currentZoom, false);
	event->accept ();
}

void
NSRPageView::onPinchCancelled ()
{
	_imageView->setPreferredSize (_initialScaleSize.width (),
				      _initialScaleSize.height ());
}

