#include "nsrreadprogress.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/LayoutUpdateHandler>

using namespace bb::cascades;

NSRReadProgress::NSRReadProgress (bb::cascades::Container *parent) :
		Container (parent),
		_progressContainer (NULL),
		_pagesCount (0),
		_currentPage (0),
		_width (0)
{
	_progressContainer = Container::create().horizontal(HorizontalAlignment::Left)
						.vertical(VerticalAlignment::Fill)
						.background(Color::fromRGBA (0, 0.66, 0.87, 1.0));

	setBackground (Color::Black);
	setLayout (DockLayout::create ());
	setPreferredHeight (10);
	setMaxHeight (10);
	setVisible (false);
	setHorizontalAlignment (HorizontalAlignment::Fill);
	add (_progressContainer);

	LayoutUpdateHandler::create(this).onLayoutFrameChanged(this,
							       SLOT (onLayoutFrameChanged (QRectF)));
}

NSRReadProgress::~NSRReadProgress ()
{
}

void
NSRReadProgress::setPagesCount (int count)
{
	if (count < 0 || _pagesCount == count)
		return;

	_pagesCount = count;
	updateProgress ();
}

int
NSRReadProgress::getPagesCount () const
{
	return _pagesCount;
}

void
NSRReadProgress::setCurrentPage (int page)
{
	if (page < 0 || _currentPage == page)
		return;

	_currentPage = page;
	updateProgress ();
}

int
NSRReadProgress::getCurrentPage () const
{
	return _currentPage;
}

void
NSRReadProgress::onLayoutFrameChanged (const QRectF& rect)
{
	_width = rect.width ();

	updateProgress ();
}

void
NSRReadProgress::updateProgress ()
{
	if (_pagesCount <= 1 || _currentPage <= 0) {
		setVisible (false);
		return;
	}

	double progress = (double) _currentPage / _pagesCount * _width;
	_progressContainer->setPreferredWidth (progress);
	setVisible (true);
}



