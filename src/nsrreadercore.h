#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"
#include "nsrrenderedpage.h"
#include "nsrpagescache.h"
#include "nsrsession.h"
#include "nsrsettings.h"
#include "nsrpageview.h"

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
	QString getDocumentPaht () const;
	NSRRenderedPage getCurrentPage () const;
	int getPagesCount () const;
	void loadPage (PageLoad dir, int pageNumber = 0);
	void setPassword (const QString &pass);
	void reloadSettings (const NSRSettings *settings);
	void loadSession (const NSRSession *session);
	bool isPageRendering () const;
	void fitToWidth (int width);
	bool isFitToWidth () const;

Q_SIGNALS:
	void pageRendered (int number);
	void needIndicator (bool enabled);
	void needPassword ();
	void errorWhileOpening (NSRAbstractDocument::DocumentError error);
	void needViewMode (NSRPageView::NSRViewMode mode);

private Q_SLOTS:
	void onRenderDone ();

private:
	NSRAbstractDocument	*_doc;
	NSRRenderThread		*_thread;
	NSRPagesCache		*_cache;
	NSRRenderedPage		_currentPage;
};

#endif /* NSRREADERCORE_H_ */
