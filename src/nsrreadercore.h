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

#include <bb/system/ApplicationStartupMode>

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

	NSRReaderCore (bb::system::ApplicationStartupMode::Type startMode,
		       QObject *parent = 0);
	virtual ~NSRReaderCore ();

	void openDocument (const QString &path);
	bool isDocumentOpened () const;
	void closeDocument ();
	QString getDocumentPath () const;

	NSRRenderedPage getCurrentPage () const;
	int getPagesCount () const;
	void setPassword (const QString &pass);
	void reloadSettings ();
	void loadSession (const NSRSession *session);
	void navigateToPage (PageLoad dir, int pageNumber = 0);
	bool isPageRendering () const;
	void setScreenWidth (int width);
	bool isFitToWidth () const;
	double getZoom () const;
	double getMinZoom () const;
	double getMaxZoom () const;
	void setZoom (double zoom, NSRRenderedPage::NSRRenderReason reason);
	void rotate (double rot);
	double getRotation () const;
	void saveCurrentPagePositions (const QPointF& pos,
				       const QPointF& textPos);
	bool isTextReflow () const;
	bool isTextReflowSwitchSupported () const;
	void switchTextReflow ();

Q_SIGNALS:
	void pageRendered (int number);
	void needIndicator (bool enabled);
	void needPassword ();
	void errorWhileOpening (NSRAbstractDocument::DocumentError error);
	void needViewMode (NSRPageView::NSRViewMode mode);
#ifdef NSR_LITE_VERSION
	void liteVersionOverPage ();
#endif

private Q_SLOTS:
	void onRenderDone ();
	void onZoomRenderDone ();
	void onZoomThreadFinished ();

private:
	void loadPage (PageLoad			dir,
		       const NSRRenderedPage&	request = NSRRenderedPage (0, NULL));
	NSRAbstractDocument * copyDocument (const NSRAbstractDocument *doc);
	NSRAbstractDocument * documentByPath (const QString& path);
	double normalizeAngle (double angle) const;

	NSRAbstractDocument				*_doc;
	NSRAbstractDocument				*_zoomDoc;
	NSRRenderThread					*_thread;
	NSRRenderZoomThread				*_zoomThread;
	NSRPagesCache					*_cache;
	NSRRenderedPage					_currentPage;
	bb::system::ApplicationStartupMode::Type	_startMode;
};

#endif /* NSRREADERCORE_H_ */
