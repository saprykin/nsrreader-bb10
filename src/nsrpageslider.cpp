#include "nsrpageslider.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrhardwareinfo.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Color>

#if BBNDK_VERSION_AT_LEAST(10,3,1)
#  include <bb/cascades/TrackpadHandler>
#endif

using namespace bb::cascades;

NSRPageSlider::NSRPageSlider (Container *parent) :
	Container (parent),
	_translator (NULL),
	_slider (NULL),
	_spaceContainer (NULL),
	_trackpadActivated (false),
	_originValue (0.0)
{
	_translator = new NSRTranslator (this);

	setLayout (StackLayout::create ());
	setVerticalAlignment (VerticalAlignment::Bottom);
	setHorizontalAlignment (HorizontalAlignment::Fill);
	setBackground (NSRThemeSupport::instance()->getOverlay ());

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	setLeftPadding (ui()->sdu (2));
	setRightPadding (ui()->sdu (2));
#else
	setLeftPadding (20);
	setRightPadding (20);
#endif

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	setTopPadding (ui()->sdu (2));
#elif BBNDK_VERSION_AT_LEAST(10,2,0)
	setTopPadding (10);
#else
	setBottomPadding (10);
	setTopPadding (30);
#endif

	_slider = Slider::create().horizontal(HorizontalAlignment::Fill)
				  .vertical(VerticalAlignment::Center)
				  .from(0)
				  .to(0);
	_slider->setLayoutProperties (StackLayoutProperties::create().spaceQuota(1.0f));

	_spaceContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Bottom)
					     .background(Color::Transparent);
	_spaceContainer->setMinHeight (0);
	_spaceContainer->setMaxHeight (0);

#if BBNDK_VERSION_AT_LEAST(10,2,0)
	_slider->accessibility()->setName (trUtf8 ("Page number slider"));

	_translator->addTranslatable ((UIObject *) _slider->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRPageSlider"),
				      QString ("Page number slider"));
#endif

	bool ok = connect (_slider, SIGNAL (touch (bb::cascades::TouchEvent *)),
			   this, SLOT (onSliderTouchEvent (bb::cascades::TouchEvent *)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	TrackpadHandler *trackpadHandler = TrackpadHandler::create()
						.onTrackpad (this, SLOT (onTrackpadEvent (bb::cascades::TrackpadEvent *)));

	_slider->addEventHandler (trackpadHandler);

	ok = connect (_slider->navigation (), SIGNAL (wantsHighlightChanged (bool)),
		      this, SLOT (onWantsHighlightChanged (bool)));
	Q_ASSERT (ok);
#endif

	add (_slider);
	add (_spaceContainer);

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      _translator, SLOT (translate ()));
	Q_ASSERT (ok);
}

NSRPageSlider::~NSRPageSlider ()
{
}

void
NSRPageSlider::setValue (int value)
{
	_slider->setValue (value);
}

void
NSRPageSlider::setRange (int from, int to)
{
	_slider->setRange (from, to);
}

void
NSRPageSlider::setBottomSpace (int space)
{
	if (space < 0)
		space = 0;

	_spaceContainer->setMinHeight (space);
	_spaceContainer->setMaxHeight (space);
}

int
NSRPageSlider::getValue () const
{
	return (int) (_slider->value () + 0.1);
}

void
NSRPageSlider::setEnabled (bool enabled)
{
	Container::setEnabled (enabled);

	if (enabled && NSRHardwareInfo::instance()->isTrackpad ()) {
#if BBNDK_VERSION_AT_LEAST(10,3,1)
		if (_slider->navigation()->wantsHighlight ())
			_slider->requestFocus ();
	}
#else
	}
#endif
}

void
NSRPageSlider::onSliderTouchEvent (bb::cascades::TouchEvent* event)
{
	if (!isEnabled ())
		return;

	switch (event->touchType ()) {
	case TouchType::Down:
		emit interactionStarted ();
		break;
	case TouchType::Up:
		_slider->setValue ((int) _slider->immediateValue ());
		emit interactionEnded ();
		break;
	case TouchType::Move:
		emit currentValueChanged ((int) _slider->immediateValue ());
		break;
	case TouchType::Cancel:
		_slider->setValue ((int) _slider->immediateValue ());
		emit interactionEnded ();
		break;
	default:
		emit interactionEnded ();
	}
}

void
NSRPageSlider::onTrackpadEvent (bb::cascades::TrackpadEvent* event)
{
#if BBNDK_VERSION_AT_LEAST(10,3,1)
	switch (event->trackpadEventType ()) {
	case TrackpadEventType::Release:
	{
		if (_trackpadActivated) {
			_slider->setValue ((int) _slider->immediateValue ());
			emit interactionEnded ();
		} else {
			_originValue = _slider->immediateValue ();
			emit interactionStarted ();
		}

		_trackpadActivated = !_trackpadActivated;
	}
	break;
	case TrackpadEventType::Move:
	{
		if (_trackpadActivated)
			emit currentValueChanged ((int) _slider->immediateValue ());
	}
	break;
	case TrackpadEventType::Cancel:
	{
		if (_trackpadActivated) {
			_slider->setValue (_originValue);
			emit currentValueChanged ((int) _slider->immediateValue ());
			emit interactionEnded ();
		}
	}
	break;
	default:
		break;
	}
#else
	Q_UNUSED (event);
#endif
}

void
NSRPageSlider::onWantsHighlightChanged (bool wantsHighlight)
{
	if (!NSRHardwareInfo::instance()->isTrackpad ())
		return;

	if (_trackpadActivated && !wantsHighlight) {
		_slider->setValue (_originValue);
		emit currentValueChanged ((int) _slider->immediateValue ());
		emit interactionEnded ();
		_trackpadActivated = false;
	}
}
