#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"

#include <QtCore/QFile>

using namespace bb::cascades;

NSRReaderCore::NSRReaderCore (QObject *parent) :
	QObject (parent),
	_doc (NULL),
	_thread (NULL),
	_page (0),
	_pagesCount (0)
{
	_thread = new NSRRenderThread (this);

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

	if (fileInfo.suffix().toLower () == "pdf")
		_doc = new NSRPopplerDocument (path);

	if (_doc == NULL)
		return;

	_pagesCount = _doc->getNumberOfPages ();

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

	_page = 0;
	_pagesCount = 0;
	_currentPage = Image ();
}

Image
NSRReaderCore::getCurrentPage () const
{
	return _currentPage;
}

int
NSRReaderCore::getCurrentPageNumber () const
{
	return _page;
}

int
NSRReaderCore::getPagesCount () const
{
	return _pagesCount;
}

void
NSRReaderCore::onRenderDone ()
{
	_currentPage = _doc->getCurrentPage ();

	emit pageRendered (_thread->getPage ());
}

void
NSRReaderCore::loadPage (PageLoad dir, int pageNumber)
{
	if (_doc == NULL)
		return;

	int pageToLoad = _page;

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
		pageToLoad = _page;
		break;
	}

	if (pageToLoad < 1 || pageToLoad > _pagesCount || pageToLoad == _page)
		return;

	_page = pageToLoad;
	_thread->setRenderContext (_doc, pageToLoad);
	_thread->start ();
}
