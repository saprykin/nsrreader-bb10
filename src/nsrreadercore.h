#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"
#include "nsrrenderzoomthread.h"
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
	QString getDocumentPath () const;
	NSRRenderedPage getCurrentPage () const;
	int getPagesCount () const;
	void setPassword (const QString &pass);
	void reloadSettings (const NSRSettings *settings);
	void loadSession (const NSRSession *session);
	void navigateToPage (PageLoad dir, int pageNumber = 0);
	bool isPageRendering () const;
	void setScreenWidth (int width);
	bool isFitToWidth () const;
	double getZoom () const;
	double getMinZoom () const;
	double getMaxZoom () const;
	void setZoom (double zoom, bool toWidth);
	void zoomIn ();
	void zoomOut ();
	void rotate (double rot);
	double getRotation () const;

Q_SIGNALS:
	void pageRendered (int number);
	void needIndicator (bool enabled);
	void needPassword ();
	void errorWhileOpening (NSRAbstractDocument::DocumentError error);
	void needViewMode (NSRPageView::NSRViewMode mode);

private Q_SLOTS:
	void onRenderDone ();
	void onZoomRenderDone ();
	void onZoomThreadFinished ();

private:
	void loadPage (PageLoad				dir,
		       NSRRenderedPage::NSRRenderReason	reason,
		       int				pageNumber = 0);
	NSRAbstractDocument * copyDocument (const NSRAbstractDocument *doc);
	NSRAbstractDocument * documentByPath (const QString& path);
	double normalizeAngle (double angle) const;

	NSRAbstractDocument	*_doc;
	NSRAbstractDocument	*_zoomDoc;
	NSRRenderThread		*_thread;
	NSRRenderZoomThread	*_zoomThread;
	NSRPagesCache		*_cache;
	NSRRenderedPage		_currentPage;
};

#endif /* NSRREADERCORE_H_ */
