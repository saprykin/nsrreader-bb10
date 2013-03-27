#ifndef NSRRENDERTHREAD_H
#define NSRRENDERTHREAD_H

#include <QThread>
#include <QList>
#include <QHash>
#include <QMutex>

#include "nsrabstractrenderthread.h"

class NSRRenderThread : public NSRAbstractRenderThread
{
	Q_OBJECT
public:
	explicit NSRRenderThread (QObject *parent = 0);
	virtual ~NSRRenderThread ();

	void setThumbnailRender (bool enabled);
	bool isThumbnailRenderEnabled () const;

	virtual void run ();

Q_SIGNALS:
	void renderDone ();

private:
	bool _renderThumbnail;
};

#endif // NSRRENDERTHREAD_H
