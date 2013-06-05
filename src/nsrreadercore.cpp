#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"
#include "nsrdjvudocument.h"
#include "nsrtiffdocument.h"
#include "nsrtextdocument.h"

#include <QFile>
#include <QFileInfo>

#include <float.h>

using namespace bb::system;

NSRReaderCore::NSRReaderCore (bb::system::ApplicationStartupMode::Type startMode,
			      QObject *parent) :
	QObject (parent),
	_doc (NULL),
	_zoomDoc (NULL),
	_thread (NULL),
	_zoomThread (NULL),
	_cache (NULL),
	_startMode (startMode)
{
	_thread		= new NSRRenderThread (this);
	_zoomThread	= new NSRRenderZoomThread (this);
	_cache		= new NSRPagesCache (this);

	connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));
	connect (_zoomThread, SIGNAL (renderDone ()), this, SLOT (onZoomRenderDone ()));
	connect (_zoomThread, SIGNAL (finished ()), this, SLOT (onZoomThreadFinished ()));

	_thread->setThumbnailRender (_startMode != ApplicationStartupMode::InvokeCard);
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
	closeDocument ();

	_doc = documentByPath (path);

	if (_doc == NULL)
		return;

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		_doc->setTextOnly (false);
		_doc->setInvertedColors (false);
	} else {
		NSRSettings settings;

		_doc->setTextOnly (settings.isWordWrap ());
		_doc->setInvertedColors (settings.isInvertedColors ());
		_doc->setEncoding (settings.getTextEncoding ());
	}

	if (!_doc->isValid ()) {
		/* Check if we need password */
		if (_doc->getLastError () == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
			emit needPassword ();
	}

	if (!_doc->isValid ()) {
		emit errorWhileOpening (_doc->getLastError ());
		delete _doc;
		_doc = NULL;
		return;
	}

	_thread->setRenderContext (_doc);

	/* We need only graphic mode to render page on zooming */
	if (_zoomDoc == NULL) {
		_zoomDoc = copyDocument (_doc);
		_zoomDoc->setTextOnly (false);
		_zoomThread->setRenderContext (_zoomDoc);
	}

	if (_startMode != ApplicationStartupMode::InvokeCard)
		NSRSettings().addLastDocument (path);
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
		_thread->setRenderContext (NULL);
	}

	/* Check whether we can delete zoom document */
	if (_zoomDoc != NULL) {
		if (!_zoomThread->isRunning ()) {
			delete _zoomDoc;
			_zoomDoc = NULL;
			_zoomThread->setRenderContext (NULL);
		} else {
			_zoomThread->setDocumentChanged (true);
			_zoomThread->cancelRequests ();
		}
	}

	_currentPage = NSRRenderedPage ();
	_cache->clearStorage ();
}

QString
NSRReaderCore::getDocumentPath () const
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

	if (wasInverted != _doc->isInvertedColors ()) {
		_cache->clearStorage ();
		needReload = true;
	}

	if (wasEncoding != _doc->getEncoding () && _doc->isEncodingUsed ())
		needReload = true;

	if (wasTextOnly != settings->isWordWrap () && !needReload)
		emit needViewMode (NSRPageView::NSR_VIEW_MODE_PREFERRED);

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
			_doc->setZoom (session->getZoomGraphic ());

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
NSRReaderCore::setScreenWidth (int width)
{
	if (width <= 0)
		return;

	if (_doc == NULL || !_doc->isValid ())
		return;

	if (_doc->isZoomToWidth () && _doc->getScreenWidth () == width)
		return;

	_doc->zoomToWidth (width);
}

bool
NSRReaderCore::isFitToWidth () const
{
	if (_doc == NULL || !_doc->isValid ())
		return false;

	return _doc->isZoomToWidth ();
}

double
NSRReaderCore::getZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getZoom ();
}

double
NSRReaderCore::getMinZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getMinZoom ();
}

double
NSRReaderCore::getMaxZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getMaxZoom ();
}

void
NSRReaderCore::setZoom (double zoom, bool toWidth)
{
	if (zoom <= 0)
		return;

	if (!toWidth && _doc->getZoom () == zoom)
		return;

	if (_doc->isTextOnly ())
		return;

	_cache->clearStorage ();

	if (!toWidth)
		_doc->setZoom (zoom);

	_zoomDoc->setZoom (zoom);

	NSRRenderedPage::NSRRenderReason reason;
	reason = toWidth ? NSRRenderedPage::NSR_RENDER_REASON_ZOOM_TO_WIDTH
			 : NSRRenderedPage::NSR_RENDER_REASON_ZOOM;

	loadPage (PAGE_LOAD_CUSTOM, reason, _currentPage.getNumber ());
}

void
NSRReaderCore::zoomIn ()
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	double newZoom = _doc->getZoom () + 25.0;

	if (newZoom > _doc->getMaxZoom ())
		newZoom = _doc->getMaxZoom ();

	setZoom (newZoom, false);
}

void
NSRReaderCore::zoomOut ()
{
	if (_doc == NULL || !_doc->isValid ())
			return;

	double newZoom = _doc->getZoom () - 25.0;

	if (newZoom < _doc->getMinZoom ())
		newZoom = _doc->getMinZoom ();

	setZoom (newZoom, false);
}

void
NSRReaderCore::rotate (double rot)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	int newRot = (int) normalizeAngle (_doc->getRotation () + rot);

	if (newRot == _doc->getRotation ())
		return;

	_doc->setRotation (newRot);

	if (_zoomDoc != NULL) {
		_zoomDoc->setRotation (_doc->getRotation ());

		if (_zoomThread != NULL && _zoomThread->isRunning ()) {
			_zoomThread->setDocumentChanged (true);
			_zoomThread->cancelRequests ();
		}
	}

	_cache->clearStorage ();

	loadPage (PAGE_LOAD_CUSTOM,
		  NSRRenderedPage::NSR_RENDER_REASON_ROTATION,
		  _currentPage.getNumber ());
}

double
NSRReaderCore::getRotation () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getRotation ();
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
	NSRRenderedPage page = _zoomThread->getRenderedPage ();

	if (!page.isValid ())
		return;

	/* We do not need to reset document changed flag because it would be
	 * done almost immediately in onZoomThreadFinished() slot */
	if (_zoomThread->isDocumentChanged ())
		return;

	_currentPage = page;

	if (!_cache->isPageExists (_currentPage.getNumber ()))
		_cache->addPage (_currentPage);

	emit pageRendered (_currentPage.getNumber ());
}

void
NSRReaderCore::onZoomThreadFinished ()
{
	if (_zoomThread->isDocumentChanged ()) {
		/* All requests must be cancelled on document opening */
		delete _zoomDoc;
		_zoomDoc = copyDocument (_doc);
		_zoomDoc->setTextOnly (false);
		_zoomThread->setRenderContext (_zoomDoc);
		_zoomThread->setDocumentChanged (false);
	}

	if (_zoomThread->hasRequests ())
		_zoomThread->start ();
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
		QString suffix = QFileInfo(_doc->getDocumentPath ()).suffix().toLower ();

		_currentPage = _cache->getPage (pageToLoad);
		emit pageRendered (pageToLoad);
		emit needViewMode (suffix == "txt" ? NSRPageView::NSR_VIEW_MODE_TEXT
						   : NSRPageView::NSR_VIEW_MODE_PREFERRED);
		return;
	}

	NSRRenderedPage request (pageToLoad);
	request.setRenderReason (reason);

	if (reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM ||
	    reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM_TO_WIDTH) {
		_zoomThread->addRequest (request);

		if (!_zoomThread->isRunning ())
			_zoomThread->start ();
	} else {
		emit needIndicator (true);

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
	res->setRotation (doc->getRotation ());

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
	else if (suffix == "djvu" || suffix == "djv")
		res = new NSRDjVuDocument (path);
	else if (suffix == "tiff" || suffix == "tif")
		res = new NSRTIFFDocument (path);
	else
		res = new NSRTextDocument (path);

	return res;
}

double
NSRReaderCore::normalizeAngle (double angle) const
{
	if (qAbs (angle) / 360.0 > 1.0)
		angle -= ((long) (angle / 360.0) * 360);

	if (angle < 0)
		angle += 360.0;
	else if (qAbs (angle - 360.0) <= DBL_EPSILON)
		angle = 0.0;

	return angle;
}



