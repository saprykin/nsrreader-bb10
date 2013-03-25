#include "nsrrenderthread.h"
#include "nsrthumbnailer.h"

#include <QMutexLocker>

NSRRenderThread::NSRRenderThread (QObject *parent) :
	QThread (parent),
	_doc (NULL),
	_renderThumbnail (false)
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
NSRRenderThread::setThumbnailRender (bool enabled)
{
	_renderThumbnail = enabled;
}

bool
NSRRenderThread::isThumbnailRenderEnabled () const
{
	return _renderThumbnail;
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

	if (!_doc->isTextOnly ()) {
		_doc->renderPage (page.getNumber ());
		page.setImage (_doc->getCurrentPage ());
		page.setZoom (_doc->getZoom ());
	}

	bool textOnly = _doc->isTextOnly ();
	_doc->setTextOnly (true);
	_doc->renderPage (page.getNumber ());
	page.setText (_doc->getText ());
	_doc->setTextOnly (textOnly);

	_renderedMutex.lock ();
	_renderedPages.append (page);
	_renderedMutex.unlock ();

	if (_renderThumbnail &&
	    !NSRThumbnailer::isThumbnailExists (_doc->getDocumentPath ())) {
		NSRRenderedPage	thumbPage;
		int		wasZoom = _doc->getZoom ();
		int		wasZoomWidth = _doc->getScreenWidth ();
		bool		wasZoomToWidth = _doc->isZoomToWidth ();

		_doc->zoomToWidth (256);
		_doc->setTextOnly (false);
		_doc->renderPage (1);

		thumbPage.setZoom (_doc->getZoom ());
		thumbPage.setImage (_doc->getCurrentPage ());

		_doc->setTextOnly (true);
		_doc->renderPage (1);

		thumbPage.setText (_doc->getText ());

		NSRThumbnailer::saveThumbnail (_doc->getDocumentPath (),
					       thumbPage);

		_doc->setTextOnly (textOnly);

		if (!wasZoomToWidth)
			_doc->setZoom (wasZoom);
		else
			_doc->zoomToWidth (wasZoomWidth);
	}

	emit renderDone ();
}
