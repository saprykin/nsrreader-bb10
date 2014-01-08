#ifndef NSRPAGESTATUS_H_
#define NSRPAGESTATUS_H_

#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/Paint>

#include <QObject>
#include <QTimerEvent>

class NSRPageStatus: public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRPageStatus (bb::cascades::Container *parent = 0);
	virtual ~NSRPageStatus ();

	void setStatus (int page, int totalPages);
	void resetStatus ();
	void setOnScreen (bool visible);
	void setAutoHide (bool autoHide);
	void setStatusBackground (const bb::cascades::Paint& paint);
	void setStatusBackgroundOpacity (float opacity);
	void setFontSize (bb::cascades::FontSize::Type size);

protected:
	void timerEvent (QTimerEvent *ev);

private:
	bb::cascades::Container	*_backgroundContainer;
	bb::cascades::Label	*_statusLabel;
	int			_timerId;
	bool			_autoHide;
};

#endif /* NSRPAGESTATUS_H_ */
