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

	bool ok = connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_zoomThread, SIGNAL (renderDone ()), this, SLOT (onZoomRenderDone ()));
	Q_ASSERT (ok);

	ok = connect (_zoomThread, SIGNAL (finished ()), this, SLOT (onZoomThreadFinished ()));
	Q_ASSERT (ok);

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
NSRReaderCore::openDocument (const QString &path,  const QString& password)
{
	closeDocument ();

	_doc = documentByPath (path);

	if (_doc == NULL)
		return;

	_doc->setPassword (password);

	if (_startMode == ApplicationStartupMode::InvokeCard)
		_doc->setInvertedColors (false);
	else {
		_doc->setInvertedColors (NSRSettings::instance()->isInvertedColors ());
		_doc->setAutoCrop (NSRSettings::instance()->isAutoCrop ());
		_doc->setEncoding (NSRSettings::instance()->getTextEncoding ());
	}

	if (!_doc->isValid ()) {
		emit errorWhileOpening (_doc->getLastError ());
		delete _doc;
		_doc = NULL;
		return;
	}

	if (_startMode != ApplicationStartupMode::InvokeCard &&
	    NSRSettings::instance()->isStarting ())
		_doc->setTextOnly (NSRSettings::instance()->isWordWrap ());
	else
		_doc->setTextOnly (_doc->getPrefferedDocumentStyle () == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);

	_thread->setRenderContext (_doc);

	/* We need only graphic mode to render page on zooming */
	if (_zoomDoc == NULL) {
		_zoomDoc = copyDocument (_doc);
		_zoomDoc->setTextOnly (false);
		_zoomThread->setRenderContext (_zoomDoc);
	}

	if (_startMode != ApplicationStartupMode::InvokeCard)
		NSRSettings::instance()->addLastDocument (path);
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
NSRReaderCore::reloadSettings ()
{
	if (_doc == NULL)
		return;

	bool	needReload = false;
	bool	wasInverted = _doc->isInvertedColors ();
	bool	wasCropped = _doc->isAutoCrop ();
	QString	wasEncoding = _doc->getEncoding ();

	_doc->setInvertedColors (NSRSettings::instance()->isInvertedColors ());
	_doc->setAutoCrop (NSRSettings::instance()->isAutoCrop ());
	_doc->setEncoding (NSRSettings::instance()->getTextEncoding ());

	if (_zoomDoc != NULL) {
		_zoomDoc->setInvertedColors (NSRSettings::instance()->isInvertedColors ());
		_zoomDoc->setAutoCrop (NSRSettings::instance()->isAutoCrop ());
	}

	if (wasInverted != _doc->isInvertedColors () ||
	    wasCropped != _doc->isAutoCrop ()) {
		/* Do not clear text from cache if text mode is remained */
		if (_doc->isTextOnly ())
			_cache->removePagesWithImages ();
		else {
			_cache->clearStorage ();
			needReload = true;
		}
	}

	if (wasEncoding != _doc->getEncoding () && _doc->isEncodingUsed ()) {
		_cache->clearStorage ();
		needReload = true;
	}

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM,
			  NSRRenderedPage (_currentPage.getNumber (),
					   NSRRenderedPage::NSR_RENDER_REASON_SETTINGS));
}

void
NSRReaderCore::loadSession (const NSRSession *session)
{
	if (session == NULL)
		return;

	QString file = session->getFile ();

	if (QFile::exists (file)) {
		openDocument (file, session->getPassword ());

		if (isDocumentOpened ()) {
			_doc->setRotation (session->getRotation ());
			_doc->setZoom (session->getZoomGraphic ());

			if (_zoomDoc != NULL)
				_zoomDoc->setRotation (session->getRotation ());

			if (session->isFitToWidth ())
				_doc->zoomToWidth (session->getZoomScreenWidth ());

			loadPage (PAGE_LOAD_CUSTOM,
				  NSRRenderedPage (session->getPage (),
						   NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION));
		}
	}
}

void
NSRReaderCore::navigateToPage (PageLoad dir, int pageNumber)
{
	loadPage (dir,
		  NSRRenderedPage (pageNumber,
				   NSRRenderedPage::NSR_RENDER_REASON_NAVIGATION));
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
NSRReaderCore::setZoom (double zoom, NSRRenderedPage::NSRRenderReason reason)
{
	if (zoom <= 0)
		return;

	bool toWidth = (reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
			reason == NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH);

	if (!toWidth && _doc->getZoom () == zoom)
		return;

	if (_doc->isTextOnly ())
		return;

	NSRRenderedPage cachedPage = _cache->getPage (_currentPage.getNumber ());
	NSRRenderedPage request (_currentPage.getNumber (), reason);
	request.setText (cachedPage.getText ());
	request.setLastTextPosition (cachedPage.getLastTextPosition ());

	if (reason == NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH)
		_cache->removePage (_currentPage.getNumber ());
	else
		_cache->clearStorage ();

	if (!toWidth)
		_doc->setZoom (zoom);

	_zoomDoc->setZoom (zoom);

	loadPage (PAGE_LOAD_CUSTOM, request);
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
		  NSRRenderedPage (_currentPage.getNumber (),
				   NSRRenderedPage::NSR_RENDER_REASON_ROTATION));
}

double
NSRReaderCore::getRotation () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getRotation ();
}

void
NSRReaderCore::saveCurrentPagePositions (const QPointF& pos,
					 const QPointF& textPos)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	_cache->updatePagePositions (_currentPage.getNumber (), pos, textPos);
}

bool
NSRReaderCore::isTextReflow () const
{
	if (!isDocumentOpened ())
		return false;

	return _doc->isTextOnly ();
}

bool
NSRReaderCore::isTextReflowSwitchSupported () const
{
	if (!isDocumentOpened ())
		return false;

	return (_doc->isDocumentStyleSupported (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) &&
		_doc->isDocumentStyleSupported (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT));
}

void
NSRReaderCore::switchTextReflow ()
{
	if (!isTextReflowSwitchSupported ())
		return;

	bool needReload = false;

	/* Check whether we need to re-render the page */
	if (_doc->isTextOnly ()) {
		_cache->removePagesWithoutImages ();
		needReload = true;
	}

	_doc->setTextOnly (!_doc->isTextOnly ());

	if (!needReload)
		emit needViewMode (_doc->isTextOnly () ? NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT
						       : NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM,
			  NSRRenderedPage (_currentPage.getNumber (),
					   NSRRenderedPage::NSR_RENDER_REASON_SETTINGS));
}

void
NSRReaderCore::onRenderDone ()
{
	_currentPage = _thread->getRenderedPage ();

	if (!_cache->isPageExists (_currentPage.getNumber ()))
		_cache->addPage (_currentPage);

	emit needIndicator (false);
	emit pageRendered (_currentPage.getNumber ());
	emit needViewMode (_doc->isTextOnly () ? NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT
					       : NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
}

void
NSRReaderCore::onZoomRenderDone ()
{
	NSRRenderedPage page = _zoomThread->getRenderedPage ();

	if (!page.isImageValid ())
		return;

	/* We do not need to reset document changed flag because it would be
	 * done almost immediately in onZoomThreadFinished() slot */
	if (_zoomThread->isDocumentChanged ())
		return;

	/* Check if the zoomed page is still relevant */
	if (_currentPage.getNumber () != page.getNumber () &&
	    qAbs (_zoomDoc->getZoom () - page.getZoom ()) <= DBL_EPSILON) {
		/* Anyway document hasn't changed, so we can save it in cache */
		_cache->addPage (page);
		return;
	}

	/* It seems that another page is rendering already */
	if (_thread->isRunning ()) {
		_zoomThread->cancelRequests ();
		return;
	}

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
NSRReaderCore::loadPage (PageLoad		dir,
			 const NSRRenderedPage&	request)
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
		pageToLoad = request.getNumber ();
		break;
	default:
		pageToLoad = _currentPage.getNumber ();
		break;
	}

	if (pageToLoad < 1)
		pageToLoad = 1;
	else if (pageToLoad > _doc->getNumberOfPages ())
		pageToLoad = _doc->getNumberOfPages ();

#ifdef NSR_LITE_VERSION
	if (pageToLoad > NSRSettings::getMaxAllowedPages ()) {
		pageToLoad = NSRSettings::getMaxAllowedPages ();
		emit liteVersionOverPage ();
	}
#endif

	NSRRenderedPage req (request);
	req.setNumber (pageToLoad);

	if (_cache->isPageExists (pageToLoad)) {
		QString suffix = QFileInfo(_doc->getDocumentPath ()).suffix().toLower ();

		_currentPage = _cache->getPage (pageToLoad);

		emit pageRendered (pageToLoad);
		emit needViewMode (_doc->isTextOnly () ? NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT
						       : NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
		return;
	}

	NSRRenderedPage::NSRRenderReason reason = req.getRenderReason ();

	if (reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM ||
	    reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
	    reason == NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH) {
		_zoomThread->addRequest (req);

		if (!_zoomThread->isRunning ())
			_zoomThread->start ();
	} else {
		emit needIndicator (true);

		_thread->addRequest (req);
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
	res->setAutoCrop (doc->isAutoCrop ());

	if (!res->isValid ()) {
		delete res;
		res = NULL;
	}

	return res;
}

NSRAbstractDocument*
NSRReaderCore::documentByPath (const QString& path) const
{
	NSRAbstractDocument	*res = NULL;
	QString			suffix = QFileInfo(path).suffix().toLower ();

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

bool
NSRReaderCore::isPasswordProtected (const QString& file) const
{
	QString			suffix = QFileInfo(file).suffix().toLower ();
	bool			res = false;

	if (suffix != "pdf")
		return res;

	NSRAbstractDocument *doc = documentByPath (file);

	if (doc->getLastError () == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
		res = true;

	delete doc;
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

