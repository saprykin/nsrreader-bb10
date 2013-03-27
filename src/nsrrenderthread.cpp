#include "nsrrenderthread.h"
#include "nsrthumbnailer.h"

#include <QMutexLocker>

NSRRenderThread::NSRRenderThread (QObject *parent) :
	NSRAbstractRenderThread (parent),
	_renderThumbnail (false)
{
}

NSRRenderThread::~NSRRenderThread ()
{
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
	NSRAbstractDocument *doc = getRenderContext ();

	if (doc == NULL) {
		emit renderDone ();
		return;
	}

	/* Does we have new pages to render? */
	NSRRenderedPage page = getRequest ();

	if (page.getNumber () < 1 ||
	    page.getNumber () > doc->getNumberOfPages ()) {
		emit renderDone ();
		return;
	}

	/* Render image only if we are in graphic mode */
	if (!doc->isTextOnly ()) {
		doc->renderPage (page.getNumber ());
		page.setImage (doc->getCurrentPage ());
		page.setZoom (doc->getZoom ());
	}

	bool textOnly = doc->isTextOnly ();
	doc->setTextOnly (true);
	doc->renderPage (page.getNumber ());
	page.setText (doc->getText ());
	doc->setTextOnly (textOnly);

	if (_renderThumbnail &&
	    !NSRThumbnailer::isThumbnailExists (doc->getDocumentPath ())) {
		NSRRenderedPage	thumbPage;
		int		wasZoom = doc->getZoom ();
		int		wasZoomWidth = doc->getScreenWidth ();
		bool		wasZoomToWidth = doc->isZoomToWidth ();

		doc->zoomToWidth (256);
		doc->setTextOnly (false);
		doc->renderPage (1);

		thumbPage.setZoom (doc->getZoom ());
		thumbPage.setImage (doc->getCurrentPage ());

		doc->setTextOnly (true);
		doc->renderPage (1);

		thumbPage.setText (doc->getText ());

		NSRThumbnailer::saveThumbnail (doc->getDocumentPath (),
				thumbPage);

		doc->setTextOnly (textOnly);

		if (!wasZoomToWidth)
			doc->setZoom (wasZoom);
		else
			doc->zoomToWidth (wasZoomWidth);
	}

	completeRequest (page);
	emit renderDone ();
}
