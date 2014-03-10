#ifndef NSRBPSEVENTHANDLER_H_
#define NSRBPSEVENTHANDLER_H_

#include <bps/bps.h>
#include <bb/AbstractBpsEventHandler>

#include <QObject>

class NSRBpsEventHandler : public QObject,
			   public bb::AbstractBpsEventHandler
{
	Q_OBJECT
public:
	NSRBpsEventHandler (QObject *parent = 0);
	~NSRBpsEventHandler ();
	virtual void event (bps_event_t *event);

	bool isVkbVisible () const {
		return _vkbVisible;
	}

Q_SIGNALS:
	void vkbVisibilityChanged (bool visible);
	void windowActiveChanged (bool active);

private:
	bool _vkbVisible;
};

#endif /* NSRBPSEVENTHANDLER_H_ */
