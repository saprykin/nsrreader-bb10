#include "nsrrenderzoomthread.h"

#include <QMutexLocker>

NSRRenderZoomThread::NSRRenderZoomThread (QObject *parent) :
	NSRAbstractRenderThread (parent)
{
}

NSRRenderZoomThread::~NSRRenderZoomThread ()
{
}

void
NSRRenderZoomThread::run ()
{
	NSRAbstractDocument	*doc = getRenderContext ();
	bool			hasPage;

	if (doc == NULL)
		return;

	do {
		/* Does we have new pages to render? */
		if (isDocumentChanged ())
			return;

		NSRRenderedPage page = getRequest ();

		if (page.getNumber () < 1 ||
		    page.getNumber () > doc->getNumberOfPages ())
			return;

		/* Last page is only one that is actual, so clear
		 * all other */
		cancelRequests ();

		/* Render image only if we are in graphic mode */
		if (!doc->isTextOnly ()) {
			doc->renderPage (page.getNumber ());
			page.setImage (doc->getCurrentPage ());
			page.setZoom (doc->getZoom ());
		}

		if (isDocumentChanged ())
			return;

		hasPage = hasRequests ();

		/* We need this page only if there is no more
		 * pages requested. If there are more pages
		 * requested then current page is outdated */
		if (!hasRequests ()) {
			completeRequest (page);
			emit renderDone ();
		}
	} while (hasPage);
}

void
NSRRenderZoomThread::setDocumentChanged (bool changed)
{
	_documentChanged = changed ? 1 : 0;
}

bool
NSRRenderZoomThread::isDocumentChanged ()
{
	return (_documentChanged == 1);
}

