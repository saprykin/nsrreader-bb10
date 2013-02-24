#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"
#include "nsrrenderedpage.h"

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
	NSRRenderedPage getCurrentPage () const;
	int getPagesCount () const;
	void loadPage (PageLoad dir, int pageNumber = 0);

Q_SIGNALS:
	void pageRendered (int number);
	void needIndicator (bool enabled);

private Q_SLOTS:
	void onRenderDone ();

private:
	NSRAbstractDocument	*_doc;
	NSRRenderThread		*_thread;
	NSRRenderedPage		_currentPage;
};

#endif /* NSRREADERCORE_H_ */
