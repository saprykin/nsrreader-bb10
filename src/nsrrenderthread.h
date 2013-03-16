#ifndef NSRRENDERTHREAD_H
#define NSRRENDERTHREAD_H

#include <QThread>
#include <QList>
#include <QHash>
#include <QMutex>

#include "nsrabstractdocument.h"
#include "nsrrenderedpage.h"

class NSRRenderThread : public QThread
{
	Q_OBJECT
public:
	explicit NSRRenderThread (QObject *parent = 0);
	~NSRRenderThread ();

	void setRenderContext (NSRAbstractDocument *doc);
	void addRequest (NSRRenderedPage &page);
	void cancelRequests ();
	NSRRenderedPage getRenderedPage ();
	void setThumbnailRender (bool enabled);
	bool isThumbnailRenderEnabled () const;

	void run ();

Q_SIGNALS:
	void renderDone ();

public Q_SLOTS:

private:
	NSRAbstractDocument		*_doc;
	QList<NSRRenderedPage>		_requestedPages;
	QList<NSRRenderedPage>		_renderedPages;
	QMutex				_requestedMutex;
	QMutex				_renderedMutex;
	bool				_renderThumbnail;
};

#endif // NSRRENDERTHREAD_H
