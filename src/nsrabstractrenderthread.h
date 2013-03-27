#ifndef NSRABSTRACTRENDERTHREAD_H_
#define NSRABSTRACTRENDERTHREAD_H_

#include "nsrrenderedpage.h"
#include "nsrabstractdocument.h"

#include <QThread>
#include <QMutex>
#include <QList>

class NSRAbstractRenderThread : public QThread
{
public:
	NSRAbstractRenderThread (QObject *parent = 0);
	virtual ~NSRAbstractRenderThread ();

	void setRenderContext (NSRAbstractDocument *doc);
	NSRAbstractDocument * getRenderContext ();
	void addRequest (NSRRenderedPage &page);
	void cancelRequests ();
	bool hasRequests ();
	NSRRenderedPage getRequest ();
	NSRRenderedPage getRenderedPage ();
	void completeRequest (const NSRRenderedPage& page);

	virtual void run () = 0;

private:
	NSRAbstractDocument		*_doc;
	QList<NSRRenderedPage>		_requestedPages;
	QList<NSRRenderedPage>		_renderedPages;
	QMutex				_requestedMutex;
	QMutex				_renderedMutex;
};

#endif /* NSRABSTRACTRENDERTHREAD_H_ */
