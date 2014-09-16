#include "nsrpagestatus.h"
#include "nsrreader.h"

#include <bb/cascades/Color>
#include <bb/system/LocaleHandler>
#include <bb/cascades/ImagePaint>
#include <bb/cascades/DockLayout>

using namespace bb::cascades;

NSRPageStatus::NSRPageStatus (bb::cascades::Container *parent) :
	Container (parent),
	_backgroundContainer (NULL),
	_statusLabel (NULL),
	_timerId (-1),
	_autoHide (true)
{
	setBackground (Color::Transparent);
	setLayout (DockLayout::create ());

	_backgroundContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						  .vertical(VerticalAlignment::Fill)
						  .background(Color::Gray)
						  .opacity(0.5);

	Container *labelContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Fill)
						       .background(Color::Transparent)
						       .layout(DockLayout::create());

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	labelContainer->setLeftPadding (ui()->sdu (0.5f));
	labelContainer->setRightPadding (ui()->sdu (0.5f));
	labelContainer->setTopPadding (ui()->sdu (0.5f));
	labelContainer->setBottomPadding (ui()->sdu (0.5f));
#else
	labelContainer->setLeftPadding (5);
	labelContainer->setRightPadding (5);
	labelContainer->setTopPadding (5);
	labelContainer->setBottomPadding (5);
#endif

	_statusLabel = Label::create().horizontal(HorizontalAlignment::Fill)
				       .vertical(VerticalAlignment::Fill);
	_statusLabel->textStyle()->setColor (Color::White);

	labelContainer->add (_statusLabel);

	add (_backgroundContainer);
	add (labelContainer);
}

NSRPageStatus::~NSRPageStatus ()
{
	if (_timerId != -1) {
		killTimer (_timerId);
		_timerId = -1;
	}
}

void
NSRPageStatus::setStatus (int page, int totalPages)
{
	bb::system::LocaleHandler region (bb::system::LocaleType::Region);

	if (page <= 0 || totalPages <= 0) {
		_statusLabel->setText (QString ());
		setVisible (false);
		return;
	}

	_statusLabel->setText (QString("%1/%2").arg(region.locale().toString (page))
					       .arg (region.locale().toString (totalPages)));
}

void NSRPageStatus::resetStatus ()
{
	_statusLabel->resetText ();
}

void
NSRPageStatus::setOnScreen (bool visible)
{
	if (visible == isVisible ())
		return;

	if (_timerId != -1) {
		killTimer (_timerId);
		_timerId = -1;
	}

	if (visible && _autoHide)
		_timerId = startTimer (3000);

	setVisible (visible);
}

void
NSRPageStatus::setAutoHide (bool autoHide)
{
	if (_autoHide == autoHide)
		return;

	if (autoHide) {
		if (_timerId == -1)
			_timerId = startTimer (3000);
	} else {
		if (_timerId != -1) {
			killTimer (_timerId);
			_timerId = -1;
		}
	}

	_autoHide = autoHide;
}

void
NSRPageStatus::setStatusBackground (const bb::cascades::Paint& paint)
{
	_backgroundContainer->setBackground (paint);
}

void
NSRPageStatus::setStatusBackgroundOpacity (float opacity)
{
	_backgroundContainer->setOpacity (opacity);
}

void
NSRPageStatus::setFontSize (bb::cascades::FontSize::Type size)
{
	_statusLabel->textStyle()->setFontSize (size);
}

void
NSRPageStatus::timerEvent (QTimerEvent* ev)
{
	if (ev->timerId () != _timerId) {
		/* wtf? kill it anyway */
		killTimer (ev->timerId ());
		return;
	}

	setOnScreen (false);
}
