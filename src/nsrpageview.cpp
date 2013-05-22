#include "nsrpageview.h"
#include "nsrsettings.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/Color>
#include <bb/cascades/TapHandler>
#include <bb/cascades/DoubleTapHandler>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/PinchHandler>
#include <bb/cascades/ImageTracker>

using namespace bb::cascades;

#define NSR_LOGO_WELCOME "asset:///nsrlogo-welcome.png"

NSRPageView::NSRPageView (Container *parent) :
	Container (parent),
	_scrollView (NULL),
	_textScrollView (NULL),
	_imageView (NULL),
	_textArea (NULL),
	_rootContainer (NULL),
	_textContainer (NULL),
	_viewMode (NSR_VIEW_MODE_GRAPHIC),
	_lastTapTime (QTime::currentTime ()),
	_currentZoom (0.0),
	_minZoom (0.0),
	_maxZoom (0.0),
	_lastTapTimer (-1),
	_initialFontSize (100),
	_isInvertedColors (false),
	_isZooming (false),
	_isZoomingEnabled (true)
{
	_scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Fill)
					  .scrollMode(ScrollMode::Both);
	_textScrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					      .vertical(VerticalAlignment::Fill)
					      .scrollMode(ScrollMode::Vertical);
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
					    .background(Color::White);
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
	_textScrollView->setContent (_textContainer);
	_textScrollView->setVisible (false);

	_initialFontSize = (int) _textArea->textStyle()->fontSize ();

	LayoutUpdateHandler::create(this).onLayoutFrameChanged (this,
							       SLOT (onLayoutFrameChanged (QRectF)));

	TapHandler *tapHandler = TapHandler::create()
					.onTapped (this, SLOT (onTapGesture (bb::cascades::TapEvent *)));
	DoubleTapHandler *dtapHandler = DoubleTapHandler::create()
				.onDoubleTapped (this, SLOT (onDoubleTappedGesture (bb::cascades::DoubleTapEvent*)));
	DoubleTapHandler *dtaptHandler = DoubleTapHandler::create()
				.onDoubleTapped (this, SLOT (onDoubleTappedGesture (bb::cascades::DoubleTapEvent*)));
	PinchHandler *pinchHandler = PinchHandler::create().onPinch (this,
						   SLOT (onPinchStarted (bb::cascades::PinchEvent *)),
						   SLOT (onPinchUpdated (bb::cascades::PinchEvent *)),
						   SLOT (onPinchEnded (bb::cascades::PinchEvent *)),
						   SLOT (onPinchCancelled ()));

	this->addGestureHandler (tapHandler);
	this->addGestureHandler (pinchHandler);
	_scrollView->addGestureHandler (dtapHandler);
	_textArea->addGestureHandler (dtaptHandler);

	setLayout (DockLayout::create ());
	add (_scrollView);
	add (_textScrollView);

	setInvertedColors (NSRSettings().isInvertedColors ());
}

NSRPageView::~NSRPageView ()
{
}

void
NSRPageView::setPage (const NSRRenderedPage& page)
{
	/* Do not accept page if we are zooming now */
	if (_isZooming)
		return;

	if (page.getRenderReason () == NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION) {
		_textArea->setText (page.getText ());
		setScrollPosition (_delayedTextScrollPos, NSR_VIEW_MODE_TEXT);
	}

	_imageView->setImage (page.getImage ());
	_imageView->setPreferredSize (page.getSize().width (), page.getSize().height ());
	_currentZoom = page.getZoom ();

	if (page.getRenderReason () == NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION ||
	    !_delayedScrollPos.isNull ())
		setScrollPosition (_delayedScrollPos, NSR_VIEW_MODE_GRAPHIC);

	_delayedScrollPos = QPointF (0, 0);
	_delayedTextScrollPos = QPointF (0, 0);
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
		_textScrollView->setVisible (false);
		_scrollView->setVisible (true);
		_viewMode = mode;
		break;
	case NSR_VIEW_MODE_TEXT:
		_textScrollView->setVisible (true);
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
NSRPageView::setZoomRange (double minZoom, double maxZoom)
{
	if (minZoom < 0)
		minZoom = 0;

	if (maxZoom < minZoom)
		maxZoom = minZoom;

	_minZoom = minZoom;
	_maxZoom = maxZoom;
}

void
NSRPageView::fitToWidth ()
{
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

int
NSRPageView::getTextZoom () const
{
	return (int) _textArea->textStyle()->fontSize ();
}

void
NSRPageView::setTextZoom (int fontSize)
{
	fontSize = qBound ((int) FontSize::XXSmall,  fontSize, (int) FontSize::XXLarge);
	fontSize = (fontSize / 10) * 10;
	_textArea->textStyle()->setFontSize ((FontSize::Type) fontSize);
}

void
NSRPageView::setZoomEnabled (bool enabled)
{
	_isZoomingEnabled = enabled;
}

bool
NSRPageView::isZoomEnabled () const
{
	return _isZoomingEnabled;
}

NSRPageView::NSRViewMode
NSRPageView::getViewMode () const
{
	return _viewMode;
}

void
NSRPageView::setScrollPosition (const QPointF& pos, NSRPageView::NSRViewMode mode)
{
	switch (mode) {
	case NSR_VIEW_MODE_GRAPHIC:
		_scrollView->scrollToPoint (pos.x (), pos.y ());
		break;
	case NSR_VIEW_MODE_TEXT:
		_textScrollView->scrollToPoint (pos.x (), pos.y ());
		break;
	case NSR_VIEW_MODE_PREFERRED:
	default:
		break;
	}
}

QPointF
NSRPageView::getScrollPosition (NSRPageView::NSRViewMode mode) const
{
	switch (mode) {
	case NSR_VIEW_MODE_GRAPHIC:
		return _scrollView->viewableArea().topLeft ();
	case NSR_VIEW_MODE_TEXT:
		return _textScrollView->viewableArea().topLeft ();
	case NSR_VIEW_MODE_PREFERRED:
	default:
		return QPointF (0, 0);
	}
}

void
NSRPageView::setScrollPositionOnLoad (const QPointF& pos, NSRPageView::NSRViewMode mode)
{
	switch (mode) {
	case NSR_VIEW_MODE_GRAPHIC:
		_delayedScrollPos = pos;
		break;
	case NSR_VIEW_MODE_TEXT:
		_delayedTextScrollPos = pos;
		break;
	case NSR_VIEW_MODE_PREFERRED:
	default:
		break;
	}
}

void NSRPageView::timerEvent (QTimerEvent* ev)
{
	killTimer (ev->timerId ());

	if (_lastTapTimer != -1 && _lastTapTime.msecsTo (QTime::currentTime ()) > 640)
		emit viewTapped ();

	_lastTapTimer = -1;
}

void
NSRPageView::onTapGesture (bb::cascades::TapEvent *ev)
{
	if (_lastTapTimer != -1) {
		killTimer (_lastTapTimer);
		_lastTapTimer = -1;
	}

	if (_lastTapTime.msecsTo (QTime::currentTime ()) > 650) {
		_lastTapTime = QTime::currentTime ();
		_lastTapTimer = startTimer (650);
	}

	ev->accept ();
}

void
NSRPageView::onDoubleTappedGesture (bb::cascades::DoubleTapEvent* ev)
{
	_lastTapTime = QTime::currentTime ();

	if (_lastTapTimer != -1) {
		killTimer (_lastTapTimer);
		_lastTapTimer = -1;
	}

	if (ev->x () < _size.width () / 3.0)
		emit prevPageRequested ();
	else if (ev->x () > _size.width () * 2 / 3)
		emit nextPageRequested ();
	else
		emit fitToWidthRequested ();

	ev->accept ();
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
	if (!_isZoomingEnabled)
		return;

	if (_viewMode == NSR_VIEW_MODE_TEXT)
		_initialFontSize = (int) _textArea->textStyle()->fontSize ();
	else {
		if (_imageView->imageSource().path () == NSR_LOGO_WELCOME)
			return;

		_initialScaleSize = QSize (_imageView->preferredWidth (),
					   _imageView->preferredHeight ());
	}

	_isZooming = true;
	event->accept ();
}

void
NSRPageView::onPinchUpdated (bb::cascades::PinchEvent* event)
{
	if (!_isZooming || !_isZoomingEnabled)
		return;

	double scale = event->pinchRatio ();

	if (_viewMode == NSR_VIEW_MODE_TEXT) {
		if (scale < 1.0)
			scale = -(1 / scale);

		int newSize = (int) (_initialFontSize + scale * 10);
		setTextZoom (newSize);
	} else {
		if (scale * _currentZoom < _minZoom)
			scale = ((double) _minZoom) / _currentZoom;
		else if (scale * _currentZoom > _maxZoom)
			scale = (double) _maxZoom / _currentZoom;

		_imageView->setPreferredSize (_initialScaleSize.width () * scale,
					      _initialScaleSize.height () * scale);
	}

	event->accept ();
}

void
NSRPageView::onPinchEnded (bb::cascades::PinchEvent* event)
{
	if (!_isZoomingEnabled)
		return;

	_isZooming = false;

	if (_viewMode == NSR_VIEW_MODE_GRAPHIC) {
		double scale = _imageView->preferredWidth () / _initialScaleSize.width ();
		_currentZoom *= scale;

		emit zoomChanged (_currentZoom, false);
	}

	event->accept ();
}

void
NSRPageView::onPinchCancelled ()
{
	if (!_isZoomingEnabled)
		return;

	_imageView->setPreferredSize (_initialScaleSize.width (),
				      _initialScaleSize.height ());
	_isZooming = false;
}

