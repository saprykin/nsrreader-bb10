#ifndef NSRRENDERZOOMTHREAD_H_
#define NSRRENDERZOOMTHREAD_H_

#include "nsrabstractrenderthread.h"

#include <QAtomicInt>

class NSRRenderZoomThread : public NSRAbstractRenderThread
{
	Q_OBJECT
public:
	NSRRenderZoomThread (QObject *parent = 0);
	virtual ~NSRRenderZoomThread ();

	virtual void run ();
	void setDocumentChanged (bool changed);
	bool isDocumentChanged ();

Q_SIGNALS:
	void renderDone ();

private:
	QAtomicInt	_documentChanged;
};

#endif /* NSRRENDERZOOMTHREAD_H_ */
