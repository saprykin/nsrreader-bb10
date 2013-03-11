#ifndef NSRPAGESTATUS_H_
#define NSRPAGESTATUS_H_

#include <bb/cascades/Container>
#include <bb/cascades/Label>

#include <QObject>
#include <QTimerEvent>

class NSRPageStatus: public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRPageStatus (Container *parent = 0);
	virtual ~NSRPageStatus ();

	void setStatus (int page, int totalPages);
	void setOnScreen (bool visible);

protected:
	void timerEvent (QTimerEvent *ev);

private:
	bb::cascades::Label	*_statusLabel;
	int			_timerId;
};

#endif /* NSRPAGESTATUS_H_ */
