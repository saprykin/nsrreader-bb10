#include "nsrrenderzoomthread.h"

NSRRenderZoomThread::NSRRenderZoomThread (QObject *parent) :
	NSRRenderThread (parent),
	_documentChanged (false)
{
}

NSRRenderZoomThread::~NSRRenderZoomThread ()
{
}

void
NSRRenderZoomThread::run ()
{
	bool hasPage;
	bool docChanged;

	if (_doc == NULL)
		return;

	do {
		/* Does we have new pages to render? */
		_requestedMutex.lock ();
		if (_requestedPages.isEmpty () || _documentChanged) {
			_requestedMutex.unlock ();
			return;
		}

		/* Last page is only one that is actual, so clear
		 * all other */
		NSRRenderedPage page = _requestedPages.takeLast ();
		_requestedPages.clear ();
		_requestedMutex.unlock ();

		/* There is a quite small probability that a new
		 * page will be requested after checking condition
		 * below and we willn't render it */
		if (page.getNumber () <= 0 ||
		    page.getNumber () > _doc->getNumberOfPages ()) {
			hasPage = true;
			continue;
		}

		/* Render image only if we are in graphic mode */
		if (!_doc->isTextOnly ()) {
			_doc->renderPage (page.getNumber ());
			page.setImage (_doc->getCurrentPage ());
			page.setZoom (_doc->getZoom ());
		}

		_requestedMutex.lock ();
		hasPage = !_requestedPages.isEmpty ();
		docChanged = _documentChanged;
		_requestedMutex.unlock ();

		if (docChanged)
			return;

		/* We need this page only if there is no more
		 * pages requested. If there are more pages
		 * requested than current page is outdated */
		if (!hasPage) {
			_renderedMutex.lock ();
			_renderedPages.append (page);
			_renderedMutex.unlock ();

			emit renderDone ();
		}
	} while (hasPage);
}

void
NSRRenderZoomThread::setDocumentChanged (bool changed)
{
	_requestedMutex.lock ();
	_documentChanged = changed;
	_requestedMutex.unlock ();
}

bool
NSRRenderZoomThread::isDocumentChanged ()
{
	QMutexLocker locker (&_requestedMutex);
	return _documentChanged;
}

bool
NSRRenderZoomThread::hasRequestedPages ()
{
	QMutexLocker locker (&_requestedMutex);
	return !_requestedPages.isEmpty ();
}





