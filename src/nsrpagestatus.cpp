#include "nsrpagestatus.h"

#include <bb/cascades/Color>
#include <bb/system/LocaleHandler>
#include <bb/cascades/ImagePaint>
#include <bb/cascades/DockLayout>

using namespace bb::cascades;

NSRPageStatus::NSRPageStatus (Container *parent) :
	Container (parent),
	_statusLabel (NULL),
	_timerId (-1)
{
	_statusLabel = Label::create().horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center);
	_statusLabel->textStyle()->setColor (Color::White);
	_statusLabel->textStyle()->setFontSize (FontSize::Small);

	setLayout (DockLayout::create ());
	setBackground (ImagePaint (QUrl ("asset:///border-white.amd")));
	setOpacity (0.6);
	setLeftPadding (10);
	setRightPadding (10);
	setTopPadding (5);
	setBottomPadding (10);
	setLeftMargin (50);

	add (_statusLabel);
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

void
NSRPageStatus::setOnScreen (bool visible)
{
	if (visible == isVisible ())
		return;

	if (_timerId != -1) {
		killTimer (_timerId);
		_timerId = -1;
	}

	if (visible)
		_timerId = startTimer (3000);

	setVisible (visible);
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



