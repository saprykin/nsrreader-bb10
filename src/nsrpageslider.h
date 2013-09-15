#ifndef NSRPAGESLIDER_H_
#define NSRPAGESLIDER_H_

#include <bb/cascades/Container>
#include <bb/cascades/Slider>
#include <bb/cascades/TouchEvent>

#include <QObject>

class NSRPageSlider : public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRPageSlider (bb::cascades::Container *parent = 0);
	virtual ~NSRPageSlider ();

	void setValue (int val);
	void setRange (int from, int to);
	void setBottomSpace (int space);
	int getValue () const;

Q_SIGNALS:
	void interactionStarted ();
	void currentValueChanged (int page);
	void interactionEnded ();

private Q_SLOTS:
	void onSliderTouchEvent (bb::cascades::TouchEvent *event);

private:
	bb::cascades::Slider *		_slider;
	bb::cascades::Container *	_spaceContainer;
};

#endif /* NSRPAGESLIDER_H_ */
