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
	_thread (NULL),
	_cache (NULL)
{
	_thread = new NSRRenderThread (this);
	_cache = new NSRPagesCache (this);

	connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));

	_thread->setThumbnailRender (true);
}

NSRReaderCore::~NSRReaderCore ()
{
	if (_doc != NULL)
		closeDocument ();
}

void
NSRReaderCore::openDocument (const QString &path)
{
	QFileInfo	fileInfo (path);
	NSRSettings	settings;

	if (!fileInfo.exists ())
		return;

	closeDocument ();

	QString suffix = fileInfo.suffix().toLower ();

	if (suffix == "pdf")
		_doc = new NSRPopplerDocument (path);
	else if (suffix == "djvu")
		_doc = new NSRDjVuDocument (path);
	else if (suffix == "tiff" || suffix == "tif")
		_doc = new NSRTIFFDocument (path);
	else
		_doc = new NSRTextDocument (path);

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
		loadPage (PAGE_LOAD_CUSTOM, _currentPage.getNumber ());
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
			loadPage (PAGE_LOAD_CUSTOM, session->getPage ());
		}
	}
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

	if (_doc->isZoomToWidth() && _doc->getScreenWidth () == width)
		return;

	_cache->clearStorage ();
	_doc->zoomToWidth (width);

	loadPage (PAGE_LOAD_CUSTOM, _currentPage.getNumber ());
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
NSRReaderCore::loadPage (PageLoad dir, int pageNumber)
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

	emit needIndicator (true);

	_thread->setRenderContext (_doc);
	_thread->addRequest (request);
	_thread->start ();
}
