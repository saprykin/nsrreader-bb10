#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"
#include "nsrdjvudocument.h"
#include "nsrtiffdocument.h"
#include "nsrtextdocument.h"

#include <QFile>
#include <QFileInfo>

NSRReaderCore::NSRReaderCore (QObject *parent) :
	QObject (parent),
	_doc (NULL),
	_zoomDoc (NULL),
	_thread (NULL),
	_zoomThread (NULL),
	_cache (NULL)
{
	_thread		= new NSRRenderThread (this);
	_zoomThread	= new NSRRenderThread (this);
	_cache		= new NSRPagesCache (this);

	connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));
	connect (_zoomThread, SIGNAL (renderDone ()), this, SLOT (onZoomRenderDone ()));

	_thread->setThumbnailRender (true);
	_zoomThread->setThumbnailRender (false);
}

NSRReaderCore::~NSRReaderCore ()
{
	if (_doc != NULL)
		closeDocument ();

	/* Force zoom thread termination */
	if (_zoomDoc != NULL && _zoomThread->isRunning ()) {
		_zoomThread->terminate ();
		delete _zoomDoc;
	}
}

void
NSRReaderCore::openDocument (const QString &path)
{
	NSRSettings settings;

	closeDocument ();


	_doc = documentByPath (path);

	if (_doc == NULL)
		return;

	_doc->setTextOnly (settings.isWordWrap ());
	_doc->setInvertedColors (settings.isInvertedColors ());
	_doc->setEncoding (settings.getTextEncoding ());

	if (!_doc->isValid ()) {
		/* Check if we need password */
		if (_doc->getLastError () == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
			emit needPassword ();
	}

	if (!_doc->isValid ()) {
		emit errorWhileOpening (_doc->getLastError ());
		delete _doc;
		_doc = NULL;
	}

	/* We need only graphic mode to render page on zooming */
	_zoomDoc = copyDocument (_doc);
	_zoomDoc->setTextOnly (false);

	settings.addLastDocument (path);
}

bool
NSRReaderCore::isDocumentOpened () const
{
	if (_doc == NULL)
		return false;

	return _doc->isValid ();
}

void
NSRReaderCore::closeDocument ()
{
	if (_doc != NULL) {
		delete _doc;
		_doc = NULL;
	}

	/* Check whether we can delete zoom document */
	if (_zoomDoc != NULL && !_zoomThread->isRunning ()) {
		delete _zoomDoc;
		_zoomDoc = NULL;
	}

	_currentPage = NSRRenderedPage ();
	_cache->clearStorage ();
}

QString
NSRReaderCore::getDocumentPaht () const
{
	if (_doc == NULL)
		return QString ();
	else
		return _doc->getDocumentPath ();
}

NSRRenderedPage
NSRReaderCore::getCurrentPage () const
{
	return _currentPage;
}

int
NSRReaderCore::getPagesCount () const
{
	if (_doc == NULL)
		return 0;

	return _doc->getNumberOfPages ();
}

void
NSRReaderCore::setPassword (const QString& pass)
{
	if (_doc != NULL)
		_doc->setPassword (pass);
}

void
NSRReaderCore::reloadSettings (const NSRSettings* settings)
{
	if (_doc == NULL)
		return;

	bool	needReload = false;
	bool	wasTextOnly = _doc->isTextOnly ();
	bool	wasInverted = _doc->isInvertedColors ();
	QString	wasEncoding = _doc->getEncoding ();

	_doc->setInvertedColors (settings->isInvertedColors ());
	_doc->setTextOnly (settings->isWordWrap ());
	_doc->setEncoding (settings->getTextEncoding ());

	if (_zoomDoc != NULL)
		_zoomDoc->setInvertedColors (settings->isInvertedColors ());

	/* Check whether we need to re-render the page */
	if (wasTextOnly && !settings->isWordWrap ())
		needReload = true;
	else if (wasTextOnly != settings->isWordWrap ())
		emit needViewMode (NSRPageView::NSR_VIEW_MODE_PREFERRED);

	if (wasInverted != _doc->isInvertedColors ()) {
		_cache->clearStorage ();
		needReload = true;
	}

	if (wasEncoding != _doc->getEncoding () && _doc->isEncodingUsed ())
		needReload = true;

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM,
			  NSRRenderedPage::NSR_RENDER_REASON_SETTINGS,
			  _currentPage.getNumber ());
}

void
NSRReaderCore::loadSession (const NSRSession *session)
{
	if (session == NULL)
		return;

	QString file = session->getFile ();

	if (QFile::exists (file)) {
		openDocument (file);

		if (isDocumentOpened ()) {
			_doc->setRotation (session->getRotation ());

			if (_zoomDoc != NULL)
				_zoomDoc->setRotation (session->getRotation ());

			if (session->isFitToWidth ())
				_doc->zoomToWidth (session->getZoomScreenWidth ());

			loadPage (PAGE_LOAD_CUSTOM,
				  NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION,
				  session->getPage ());
		}
	}
}

void
NSRReaderCore::navigateToPage (PageLoad dir, int pageNumber)
{
	loadPage (dir, NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION, pageNumber);
}


bool
NSRReaderCore::isPageRendering () const
{
	return _thread->isRunning ();
}

void
NSRReaderCore::fitToWidth (int width)
{
	if (width <= 0)
		return;

	if (_doc == NULL || !_doc->isValid ())
		return;

	if (_doc->isZoomToWidth () && _doc->getScreenWidth () == width)
		return;

	_cache->clearStorage ();
	_doc->zoomToWidth (width);

	loadPage (PAGE_LOAD_CUSTOM,
		  NSRRenderedPage::NSR_RENDER_REASON_ZOOM,
		  _currentPage.getNumber ());
}

bool
NSRReaderCore::isFitToWidth () const
{
	if (_doc == NULL || !_doc->isValid ())
		return false;

	return _doc->isZoomToWidth ();
}

int
NSRReaderCore::getZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getZoom ();
}

int
NSRReaderCore::getMinZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getMinZoom ();
}

int
NSRReaderCore::getMaxZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getMaxZoom ();
}

void
NSRReaderCore::setZoom (int zoom)
{
	if (zoom <= 0)
		return;

	if (_doc->getZoom () == zoom)
		return;

	_cache->clearStorage ();
	_doc->setZoom (zoom);
	_zoomDoc->setZoom (zoom);

	if (!_doc->isTextOnly ())
		loadPage (PAGE_LOAD_CUSTOM,
			  NSRRenderedPage::NSR_RENDER_REASON_ZOOM,
			  _currentPage.getNumber ());
}

void
NSRReaderCore::onRenderDone ()
{
	QString suffix = QFileInfo(_doc->getDocumentPath ()).suffix().toLower ();

	_currentPage = _thread->getRenderedPage ();

	if (!_cache->isPageExists (_currentPage.getNumber ()))
		_cache->addPage (_currentPage);

	emit needIndicator (false);
	emit pageRendered (_currentPage.getNumber ());
	emit needViewMode (suffix == "txt" ? NSRPageView::NSR_VIEW_MODE_TEXT
					   : NSRPageView::NSR_VIEW_MODE_PREFERRED);
}

void
NSRReaderCore::onZoomRenderDone ()
{
	QString suffix = QFileInfo(_doc->getDocumentPath ()).suffix().toLower ();

	/* TODO: check whether we need to drop outdated page */
	_currentPage = _zoomThread->getRenderedPage ();

	if (!_cache->isPageExists (_currentPage.getNumber ()))
		_cache->addPage (_currentPage);

	emit pageRendered (_currentPage.getNumber ());
}

void
NSRReaderCore::loadPage (PageLoad				dir,
			 NSRRenderedPage::NSRRenderReason	reason,
			 int					pageNumber)
{
	if (_doc == NULL || _thread->isRunning ())
		return;

	int pageToLoad = _currentPage.getNumber ();

	switch (dir) {
	case PAGE_LOAD_PREV:
		pageToLoad -= 1;
		break;
	case PAGE_LOAD_NEXT:
		pageToLoad += 1;
		break;
	case PAGE_LOAD_CUSTOM:
		pageToLoad = pageNumber;
		break;
	default:
		pageToLoad = _currentPage.getNumber ();
		break;
	}

	if (pageToLoad < 1)
		pageToLoad = 1;
	else if (pageToLoad > _doc->getNumberOfPages ())
		pageToLoad = _doc->getNumberOfPages ();

	if (_cache->isPageExists (pageToLoad)) {
		_currentPage = _cache->getPage (pageToLoad);
		emit pageRendered (pageToLoad);
		return;
	}

	NSRRenderedPage request (pageToLoad);
	request.setRenderReason (reason);

	if (reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM) {
		_zoomThread->setRenderContext (_zoomDoc);
		_zoomThread->addRequest (request);
		_zoomThread->start ();
	} else {
		emit needIndicator (true);

		_thread->setRenderContext (_doc);
		_thread->addRequest (request);
		_thread->start ();
	}
}

NSRAbstractDocument*
NSRReaderCore::copyDocument (const NSRAbstractDocument* doc)
{
	NSRAbstractDocument *res;

	if (doc == NULL || !doc->isValid ())
		return NULL;

	res = documentByPath (doc->getDocumentPath ());

	if (res == NULL)
		return NULL;

	res->setTextOnly (doc->isTextOnly ());
	res->setInvertedColors (doc->isInvertedColors ());
	res->setEncoding (doc->getEncoding ());
	res->setPassword (doc->getPassword ());

	if (!res->isValid ()) {
		delete res;
		res = NULL;
	}

	return res;
}

NSRAbstractDocument*
NSRReaderCore::documentByPath (const QString& path)
{
	NSRAbstractDocument	*res = NULL;
	QFileInfo		fileInfo (path);
	QString			suffix = fileInfo.suffix().toLower ();

	if (!QFile::exists (path))
		return NULL;

	if (suffix == "pdf")
		res = new NSRPopplerDocument (path);
	else if (suffix == "djvu")
		res = new NSRDjVuDocument (path);
	else if (suffix == "tiff" || suffix == "tif")
		res = new NSRTIFFDocument (path);
	else
		res = new NSRTextDocument (path);

	return res;
}


