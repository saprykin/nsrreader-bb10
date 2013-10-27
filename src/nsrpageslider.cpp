#include "nsrpageslider.h"
#include "nsrglobalnotifier.h"
#include "nsrreader.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Color>

using namespace bb::cascades;

NSRPageSlider::NSRPageSlider (Container *parent) :
	Container (parent),
	_translator (NULL),
	_slider (NULL),
	_spaceContainer (NULL)
{
	_translator = new NSRTranslator (this);

	setLayout (StackLayout::create ());
	setVerticalAlignment (VerticalAlignment::Bottom);
	setHorizontalAlignment (HorizontalAlignment::Fill);
	setBackground (Color::fromRGBA (0.1f, 0.1f, 0.1f, 0.95f));

	setLeftPadding (20);
	setRightPadding (20);
#ifdef BBNDK_VERSION_AT_LEAST
#  if !BBNDK_VERSION_AT_LEAST(10,2,0)
	setBottomPadding (10);
	setTopPadding (30);
#  else
	setTopPadding (10);
#  endif
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

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	_slider->accessibility()->setName (trUtf8 ("Page number slider"));

	_translator->addTranslatable ((UIObject *) _slider->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRPageSlider"),
				      QString ("Page number slider"));
#  endif
#endif

	bool ok = connect (_slider, SIGNAL (touch (bb::cascades::TouchEvent *)),
		      this, SLOT (onSliderTouchEvent (bb::cascades::TouchEvent *)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

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
