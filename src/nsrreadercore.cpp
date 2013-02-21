#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"

#include <QtCore/QFile>

using namespace bb::cascades;

NSRReaderCore::NSRReaderCore (QObject *parent) :
	QObject (parent),
	_doc (NULL),
	_currentPage (NULL)
{
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

	_doc->renderPage (5);
	_currentPage = _doc->getCurrentPage ();
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
}

Image *
NSRReaderCore::getCurrentPage ()
{
	return _currentPage;
}
