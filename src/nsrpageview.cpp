#include "nsrpageview.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrreader.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/Color>
#include <bb/cascades/TapHandler>
#include <bb/cascades/DoubleTapHandler>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/PinchHandler>
#include <bb/cascades/ImageTracker>
#include <bb/cascades/ActionItem>
#include <bb/cascades/UIOrientation>
#include <bb/cascades/OrientationSupport>

#include <bb/device/DisplayInfo>

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,2,0)
#    include <bb/cascades/ScrollRailsPolicy>
#endif

using namespace bb::cascades;
using namespace bb::device;

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
#  define NSR_PAGEVIEW_WIDTH_THRESHOLD	(ui()->sdu (1) / 2)
#else
#  define NSR_PAGEVIEW_WIDTH_THRESHOLD	4
#endif

#define NSR_PAGEVIEW_ZOOM_IN_RATIO	1.1
#define NSR_PAGEVIEW_ZOOM_OUT_RATIO	0.9
#define NSR_PAGEVIEW_SCALE_THRESHOLD	0.05

NSRPageView::NSRPageView (Container *parent) :
	Container (parent),
	_translator (NULL),
	_scrollView (NULL),
	_textScrollView (NULL),
	_imageView (NULL),
	_textArea (NULL),
	_rootContainer (NULL),
	_textContainer (NULL),
	_imageContainer (NULL),
	_actionSet (NULL),
	_viewMode (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC),
	_lastTapTime (QTime::currentTime ()),
	_currentZoom (0.0),
	_minZoom (0.0),
	_maxZoom (0.0),
	_lastTapTimer (-1),
	_initialFontSize (100),
	_isInvertedColors (false),
	_isZooming (false),
	_isZoomingEnabled (true),
	_isActionsEnabled (true),
	_isGesturesEnabled (true)
{
	_translator = new NSRTranslator (this);

	_scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Fill)
					  .scrollMode(ScrollMode::Both);
	_textScrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					      .vertical(VerticalAlignment::Fill)
					      .scrollMode(ScrollMode::Vertical);
	_imageView = ImageView::create().horizontal(HorizontalAlignment::Center)
					.vertical(VerticalAlignment::Center);

	_imageContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Fill)
					     .layout(DockLayout::create ())
					     .background(NSRThemeSupport::instance()->getBackground ());
	_imageContainer->add (_imageView);
	_scrollView->setContent (_imageContainer);

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
	_textArea->setTextFormat (TextFormat::Plain);
	_textArea->textStyle()->setColor (Color::Black);
	_textContainer->add (_textArea);
	_textScrollView->setContent (_textContainer);
	_textScrollView->setVisible (false);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,2,0)
	_scrollView->scrollViewProperties()->setScrollRailsPolicy (ScrollRailsPolicy::LockNearAxes);
#endif

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

	ActionItem *rotateLeftAction = ActionItem::create().title (trUtf8 ("Rotate Left"));
	rotateLeftAction->setImageSource (QUrl ("asset:///rotate-left.png"));
	ActionItem *rotateRightAction = ActionItem::create().title (trUtf8 ("Rotate Right"));
	rotateRightAction->setImageSource (QUrl ("asset:///rotate-right.png"));
	ActionItem *fitToWidthAction = ActionItem::create().title (trUtf8 ("Fit to Width",
								  	   "Fit image to screen width"));
	fitToWidthAction->setImageSource (QUrl ("asset:///fit-to-width.png"));

	_actionSet = ActionSet::create ();
	_actionSet->add (fitToWidthAction);
	_actionSet->add (rotateLeftAction);
	_actionSet->add (rotateRightAction);

	bool ok = connect (rotateLeftAction, SIGNAL (triggered ()), this, SIGNAL (rotateLeftRequested ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (rotateRightAction, SIGNAL (triggered ()), this, SIGNAL (rotateRightRequested ()));
	Q_ASSERT (ok);

	ok = connect (fitToWidthAction, SIGNAL (triggered ()), this, SIGNAL (fitToWidthRequested ()));
	Q_ASSERT (ok);

	_scrollView->addActionSet (_actionSet);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,2,0)
	_textArea->accessibility()->setName (trUtf8 ("Page text"));
	_imageView->accessibility()->setName (trUtf8 ("Page image"));
#endif

	setLayout (DockLayout::create ());
	add (_scrollView);
	add (_textScrollView);

	/* Fill control size with default value (the whole screen) */
	QSize displaySize = DisplayInfo().pixelSize ();

	if (OrientationSupport::instance()->orientation () == UIOrientation::Landscape)
		displaySize.transpose ();

	_size = displaySize;

	retranslateUi ();

	_translator->addTranslatable ((UIObject *) rotateLeftAction,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRPageView"),
				      QString ("Rotate Left"));
	_translator->addTranslatable ((UIObject *) rotateRightAction,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRPageView"),
				      QString ("Rotate Right"));
	_translator->addTranslatable ((UIObject *) fitToWidthAction,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRPageView"),
				      QString ("Fit to Width"));
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,2,0)
	_translator->addTranslatable ((UIObject *) _textArea->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRPageView"),
				      QString ("Page text"));
	_translator->addTranslatable ((UIObject *) _imageView->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRPageView"),
				      QString ("Page image"));
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      this, SLOT (retranslateUi ()));
	Q_ASSERT (ok);
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

	if (_isActionsEnabled && _scrollView->actionSetCount () == 0)
		_scrollView->addActionSet (_actionSet);

	if (_delayedScrollPos.isNull ())
		_delayedScrollPos = page.getLastPosition ();

	if (_delayedTextScrollPos.isNull ())
		_delayedTextScrollPos = page.getLastTextPosition ();

	if (page.getRenderReason () == NSRRenderRequest::NSR_RENDER_REASON_SETTINGS) {
		/* Encoding may be changed, we must reload text data */
		QPointF pos = getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
		_textScrollView->scrollToPoint (0, 0, ScrollAnimation::None);
		_textArea->setText (page.getText().isEmpty () ? trUtf8 ("No text data available for this page")
							      : page.getText ());
		setScrollPosition (pos, NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
	} else if (page.getRenderReason () == NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION) {
		/* Set scroll position for graphic mode */
		if (_delayedScrollPos.isNull () && _page.getNumber () != page.getNumber ())
			_delayedScrollPos = QPointF (_scrollView->viewableArea().left (), 0);

		_textScrollView->scrollToPoint (0, 0, ScrollAnimation::None);
		_textArea->setText (page.getText().isEmpty () ? trUtf8 ("No text data available for this page")
							      : page.getText ());
		setScrollPosition (_delayedTextScrollPos, NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
	}

	_imageView->setImage (page.getImage ());
	_imageView->setPreferredSize (page.getSize().width (), page.getSize().height ());
	_currentZoom = page.getRenderedZoom ();

	if (page.getRenderReason () == NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION ||
	    !_delayedScrollPos.isNull ())
		setScrollPosition (_delayedScrollPos, NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);

	if (page.getRenderReason () == NSRRenderRequest::NSR_RENDER_REASON_ROTATION)
		_scrollView->scrollToPoint (0, 0, ScrollAnimation::None);

	_delayedScrollPos = QPointF (0, 0);
	_delayedTextScrollPos = QPointF (0, 0);
	_page = page;

	retranslateTitle ();
}

void
NSRPageView::resetPage ()
{
	_imageView->resetImage ();
	_imageView->resetImageSource ();
	_page = NSRRenderedPage ();
	_scrollView->removeActionSet (_actionSet);
	_textArea->resetText ();
	_currentZoom = 100;
}

void
NSRPageView::setViewMode (NSRAbstractDocument::NSRDocumentStyle mode)
{
	if (_viewMode == mode)
		return;

	switch (mode) {
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC:
		_textScrollView->setVisible (false);
		_scrollView->setVisible (true);
		_viewMode = mode;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT:
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
NSRPageView::fitToWidth (NSRRenderRequest::NSRRenderReason reason)
{
	if (_viewMode != NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC)
		return;

	if (_imageView->image().isNull ())
		return;

	double scale = _size.width () / _imageView->preferredWidth ();

	_imageView->setImplicitLayoutAnimationsEnabled (false);
	_scrollView->setImplicitLayoutAnimationsEnabled (false);
	_imageContainer->setImplicitLayoutAnimationsEnabled (false);

	if (reason != NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH && scale < 1.0)
		_scrollView->scrollToPoint (0, 0, ScrollAnimation::None);

	_imageView->setImplicitLayoutAnimationsEnabled (true);
	_scrollView->setImplicitLayoutAnimationsEnabled (true);
	_imageContainer->setImplicitLayoutAnimationsEnabled (true);

	_imageView->setPreferredSize (_imageView->preferredWidth () * scale,
				      _imageView->preferredHeight () * scale);
	_currentZoom *= scale;

	emit zoomChanged (_currentZoom, reason);
}

int
NSRPageView::getTextZoom () const
{
	return (int) _textArea->textStyle()->fontSize ();
}

void
NSRPageView::setTextZoom (int fontSize)
{
	fontSize = qBound ((int) FontSize::XSmall, fontSize, (int) FontSize::XXLarge);
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

void
NSRPageView::setActionsEnabled (bool enabled)
{
	if (_isActionsEnabled == enabled)
		return;

	if (enabled) {
		if (_page.isValid () && _scrollView->actionSetCount () == 0)
			_scrollView->addActionSet (_actionSet);
	} else
		_scrollView->removeActionSet (_actionSet);

	_textArea->setTouchPropagationMode (enabled ? TouchPropagationMode::Full : TouchPropagationMode::None);

	_isActionsEnabled = enabled;
}

void
NSRPageView::setGesturesEnabled (bool enabled)
{
	if (_isGesturesEnabled == enabled)
		return;

	_isGesturesEnabled = enabled;
}

NSRAbstractDocument::NSRDocumentStyle
NSRPageView::getViewMode () const
{
	return _viewMode;
}

void
NSRPageView::setScrollPosition (const QPointF& pos, NSRAbstractDocument::NSRDocumentStyle mode)
{
	switch (mode) {
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC:
		_scrollView->scrollToPoint (pos.x (), pos.y ());
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT:
		_textScrollView->scrollToPoint (pos.x (), pos.y ());
		break;
	default:
		break;
	}
}

QPointF
NSRPageView::getScrollPosition (NSRAbstractDocument::NSRDocumentStyle mode) const
{
	switch (mode) {
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC:
		return _scrollView->viewableArea().topLeft ();
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT:
		return _textScrollView->viewableArea().topLeft ();
	default:
		return QPointF (0, 0);
	}
}

void
NSRPageView::setScrollPositionOnLoad (const QPointF& pos, NSRAbstractDocument::NSRDocumentStyle mode)
{
	switch (mode) {
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC:
		_delayedScrollPos = pos;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT:
		_delayedTextScrollPos = pos;
		break;
	default:
		break;
	}
}

void
NSRPageView::zoomIn ()
{
	if (!_isZoomingEnabled)
		return;

	if (_viewMode == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) {
		if (!_page.isImageValid ())
			return;

		double scale = NSR_PAGEVIEW_ZOOM_IN_RATIO;

		if (scale * _currentZoom > _maxZoom)
			scale = (double) _maxZoom / _currentZoom;

		if (qAbs (_currentZoom * scale - _currentZoom) > NSR_PAGEVIEW_SCALE_THRESHOLD) {
			QPointF center =  _scrollView->viewableArea().center () * scale;

			if (qAbs (_imageView->preferredWidth () * scale - _imageView->preferredWidth ()) >=
				  NSR_PAGEVIEW_WIDTH_THRESHOLD) {
				_imageView->setPreferredSize (_imageView->preferredWidth () * scale,
							      _imageView->preferredHeight () * scale);
				_scrollView->scrollToPoint (center.x () - _size.width () / 2,
						 	    center.y () - _size.height () / 2,
						 	    ScrollAnimation::None);
			}

			_currentZoom *= scale;
			emit zoomChanged (_currentZoom, NSRRenderRequest::NSR_RENDER_REASON_ZOOM);
		}
	} else
		setTextZoom ((int) _textArea->textStyle()->fontSize () +
			     (int) (FontSize::Medium - FontSize::Small));
}

void
NSRPageView::zoomOut ()
{
	if (!_isZoomingEnabled)
		return;

	if (_viewMode == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) {
		if (!_page.isImageValid ())
			return;

		double scale = NSR_PAGEVIEW_ZOOM_OUT_RATIO;

		if (scale * _currentZoom < _minZoom)
			scale = ((double) _minZoom) / _currentZoom;

		if (qAbs (_currentZoom * scale - _currentZoom) > NSR_PAGEVIEW_SCALE_THRESHOLD) {
			if (_imageView->preferredWidth () * scale < _size.width ())
				scale = (double) _imageView->preferredWidth () / _size.width ();

			QPointF center =  _scrollView->viewableArea().center () * scale;

			if (qAbs (_imageView->preferredWidth () * scale - _imageView->preferredWidth ()) >=
				  NSR_PAGEVIEW_WIDTH_THRESHOLD) {
				_imageView->setPreferredSize (_imageView->preferredWidth () * scale,
							      _imageView->preferredHeight () * scale);
				_scrollView->scrollToPoint (center.x () - _size.width () / 2,
							    center.y () - _size.height () / 2,
							    ScrollAnimation::None);
			}

			_currentZoom *= scale;
			emit zoomChanged (_currentZoom, NSRRenderRequest::NSR_RENDER_REASON_ZOOM);
		}
	} else
		setTextZoom ((int) _textArea->textStyle()->fontSize () -
			     (int) (FontSize::Medium - FontSize::Small));
}

void
NSRPageView::timerEvent (QTimerEvent* ev)
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
	if (!_isGesturesEnabled)
		return;

	_lastTapTime = QTime::currentTime ();

	if (_lastTapTimer != -1) {
		killTimer (_lastTapTimer);
		_lastTapTimer = -1;
	}

	if (ev->x () < _size.width () / 3.0)
		emit prevPageRequested ();
	else if (ev->x () > _size.width () * 2 / 3)
		emit nextPageRequested ();
	else {
		if (qAbs (_imageView->preferredWidth () - _size.width ()) > NSR_PAGEVIEW_WIDTH_THRESHOLD)
			emit fitToWidthRequested ();
	}

	ev->accept ();
}

void
NSRPageView::onLayoutFrameChanged (const QRectF& rect)
{
	_size = QSize ((int) rect.width (),
		       (int) rect.height ());

	emit sizeChanged (_size);
}

void
NSRPageView::onPinchStarted (bb::cascades::PinchEvent* event)
{
	if (!_isZoomingEnabled)
		return;

	if (_viewMode == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT)
		_initialFontSize = (int) _textArea->textStyle()->fontSize ();
	else {
		if (!_page.isImageValid ())
			return;

		_initialScaleSize = QSize (_imageView->preferredWidth (),
					   _imageView->preferredHeight ());
		_initialScalePos = _scrollView->viewableArea().center ();
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

	if (_viewMode == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT) {
		if (scale < 1.0)
			scale = -(1 / scale);

		int newSize = (int) (_initialFontSize + scale * 10);
		setTextZoom (newSize);
	} else {
		if (scale * _currentZoom < _minZoom)
			scale = ((double) _minZoom) / _currentZoom;
		else if (scale * _currentZoom > _maxZoom)
			scale = (double) _maxZoom / _currentZoom;

		if (scale < 1.0 && _initialScaleSize.width () * scale < _size.width ())
			scale = (double) _imageView->preferredWidth () / _initialScaleSize.width ();

		QPointF center = _initialScalePos * scale;

		if (qAbs (_initialScaleSize.width () * scale - _imageView->preferredWidth ()) >=
			  NSR_PAGEVIEW_WIDTH_THRESHOLD) {
			_imageView->setPreferredSize (_initialScaleSize.width () * scale,
						      _initialScaleSize.height () * scale);
			_scrollView->scrollToPoint (center.x () - _size.width () / 2,
						    center.y () - _size.height () / 2,
						    ScrollAnimation::None);
		}
	}

	event->accept ();
}

void
NSRPageView::onPinchEnded (bb::cascades::PinchEvent* event)
{
	if (!_isZoomingEnabled)
		return;

	_isZooming = false;

	if (_viewMode == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) {
		double scale = _imageView->preferredWidth () / _initialScaleSize.width ();

		if (qAbs (_currentZoom * scale - _currentZoom) > NSR_PAGEVIEW_SCALE_THRESHOLD) {
			if (_imageView->preferredWidth () < _size.width ()) {
				scale = (double) _size.width () / _initialScaleSize.width ();

				QPointF center = _initialScalePos * scale;

				_imageView->setPreferredSize (_initialScaleSize.width () * scale,
							      _initialScaleSize.height () * scale);
				_scrollView->scrollToPoint (center.x () - _size.width () / 2,
							    center.y () - _size.height () / 2,
							    ScrollAnimation::None);
			}


			_currentZoom *= scale;
			emit zoomChanged (_currentZoom, NSRRenderRequest::NSR_RENDER_REASON_ZOOM);
		}
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

void
NSRPageView::retranslateUi ()
{
	retranslateTitle ();

	if (_page.isValid () && _page.getText().isEmpty ())
		_textArea->setText (trUtf8 ("No text data available for this page"));

	_translator->translate ();
}

void
NSRPageView::retranslateTitle ()
{
	if (_scrollView->actionSetCount () > 0)
		_scrollView->actionSetAt(0)->setTitle (trUtf8("Page %1").arg (_page.getNumber ()));
}
