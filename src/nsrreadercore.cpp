#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"
#include "nsrdjvudocument.h"
#include "nsrtiffdocument.h"
#include "nsrtextdocument.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

NSRReaderCore::NSRReaderCore (QObject *parent) :
	QObject (parent),
	_doc (NULL),
	_thread (NULL),
	_cache (NULL)
{
	_thread = new NSRRenderThread (this);
	_cache = new NSRPagesCache (this);

	connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));
}

NSRReaderCore::~NSRReaderCore ()
{
	if (_doc != NULL)
		closeDocument ();
}

void
NSRReaderCore::openDocument (const QString &path)
{
	QFileInfo fileInfo (path);

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

	if (_doc == NULL)
		return;

	loadPage (PAGE_LOAD_CUSTOM, 1);
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

void NSRReaderCore::setPassword (const QString& pass)
{
	if (_doc != NULL)
		_doc->setPassword (pass);
}

void
NSRReaderCore::onRenderDone ()
{
	_currentPage = _thread->getRenderedPage ();

	if (!_cache->isPageExists (_currentPage.getNumber ()))
		_cache->addPage (_currentPage);

	emit needIndicator (false);
	emit pageRendered (_currentPage.getNumber ());
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

	if (pageToLoad < 1 || pageToLoad > _doc->getNumberOfPages () ||
	    pageToLoad == _currentPage.getNumber ())
		return;

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
