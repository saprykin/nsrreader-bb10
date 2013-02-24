#include "nsrrenderthread.h"

#include <QMutexLocker>

NSRRenderThread::NSRRenderThread (QObject *parent) :
	QThread (parent),
	_doc (NULL)
{
}

void
NSRRenderThread::setRenderContext (NSRAbstractDocument* doc)
{
	_doc = doc;
}

NSRRenderThread::~NSRRenderThread ()
{
	QMutexLocker locker (&_requestedMutex);
	QMutexLocker rlocker (&_renderedMutex);

	_requestedPages.clear ();
	_renderedPages.clear ();
}

void
NSRRenderThread::addRequest (NSRRenderedPage& page)
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.append (page);
}

void
NSRRenderThread::cancelRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.clear ();
}

NSRRenderedPage
NSRRenderThread::getRenderedPage ()
{
	QMutexLocker locker (&_renderedMutex);

	return _renderedPages.isEmpty () ? NSRRenderedPage () :
					  _renderedPages.takeFirst ();
}

void
NSRRenderThread::run ()
{
	QMutexLocker locker (&_requestedMutex);

	if (_doc == NULL || _requestedPages.isEmpty ()) {
		emit renderDone ();
		return;
	}

	NSRRenderedPage page = _requestedPages.takeFirst ();
	locker.unlock ();

	if (!(page.getNumber () > 0) || page.getNumber () > _doc->getNumberOfPages ())
		return;

	_doc->renderPage (page.getNumber ());
	page.setImage (_doc->getCurrentPage ());

	_doc->setTextOnly (true);
	_doc->renderPage (page.getNumber ());
	page.setText (_doc->getText ());
	_doc->setTextOnly (false);

	_renderedMutex.lock ();
	_renderedPages.append (page);
	_renderedMutex.unlock ();

	emit renderDone ();
}
