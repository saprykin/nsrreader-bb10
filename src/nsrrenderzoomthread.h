#ifndef NSRRENDERZOOMTHREAD_H_
#define NSRRENDERZOOMTHREAD_H_

#include "nsrrenderthread.h"

class NSRRenderZoomThread : public NSRRenderThread
{
	Q_OBJECT
public:
	NSRRenderZoomThread (QObject *parent = 0);
	virtual ~NSRRenderZoomThread ();

	virtual void run ();
	void setDocumentChanged (bool changed);
	bool isDocumentChanged ();
	bool hasRequestedPages ();

private:
	/* Disable thumbnailer features */
	void setThumbnailRender (bool enabled) {Q_UNUSED (enabled)}
	bool isThumbnailRenderEnabled () const {return false;}

	bool	_documentChanged;
};

#endif /* NSRRENDERZOOMTHREAD_H_ */
