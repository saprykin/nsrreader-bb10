#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include <bb/cascades/Image>
#include <bb/ImageData>

#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"

class NSRReaderCore: public QObject
{
	Q_OBJECT
	Q_ENUMS (PageLoad)
public:
	enum PageLoad {
		PAGE_LOAD_PREV		= 0,
		PAGE_LOAD_NEXT		= 1,
		PAGE_LOAD_CUSTOM	= 2
	};

	NSRReaderCore (QObject *parent = 0);
	virtual ~NSRReaderCore ();

	void openDocument (const QString &path);
	bool isDocumentOpened () const;
	void closeDocument ();
	bb::cascades::Image getCurrentPage () const;
	int getCurrentPageNumber () const;
	int getPagesCount () const;
	void loadPage (PageLoad dir, int pageNumber = 0);

Q_SIGNALS:
	void pageRendered (int number);

private Q_SLOTS:
	void onRenderDone ();

private:
	NSRAbstractDocument	*_doc;
	NSRRenderThread		*_thread;
	bb::cascades::Image	_currentPage;
	int			_page;
	int			_pagesCount;
};

#endif /* NSRREADERCORE_H_ */
