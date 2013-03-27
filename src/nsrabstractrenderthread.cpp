#include "nsrabstractrenderthread.h"

#include <QMutexLocker>

NSRAbstractRenderThread::NSRAbstractRenderThread (QObject *parent) :
	QThread (parent),
	_doc (NULL)
{
}

NSRAbstractRenderThread::~NSRAbstractRenderThread ()
{
	QMutexLocker locker (&_requestedMutex);
	QMutexLocker rlocker (&_renderedMutex);

	_requestedPages.clear ();
	_renderedPages.clear ();
}

void
NSRAbstractRenderThread::setRenderContext (NSRAbstractDocument* doc)
{
	_doc = doc;
}

NSRAbstractDocument *
NSRAbstractRenderThread::getRenderContext ()
{
	return _doc;
}

void
NSRAbstractRenderThread::addRequest (NSRRenderedPage& page)
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.append (page);
}

void
NSRAbstractRenderThread::cancelRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.clear ();
}

bool
NSRAbstractRenderThread::hasRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	return !_requestedPages.isEmpty ();
}

NSRRenderedPage
NSRAbstractRenderThread::getRequest ()
{
	QMutexLocker locker (&_requestedMutex);

	if (_requestedPages.isEmpty ())
		return NSRRenderedPage ();
	else
		return _requestedPages.takeLast ();
}

NSRRenderedPage
NSRAbstractRenderThread::getRenderedPage ()
{
	QMutexLocker locker (&_renderedMutex);

	return _renderedPages.isEmpty () ? NSRRenderedPage () :
					  _renderedPages.takeFirst ();
}

void
NSRAbstractRenderThread::completeRequest (const NSRRenderedPage& page)
{
	_renderedMutex.lock ();
	_renderedPages.append (page);
	_renderedMutex.unlock ();
}



