#ifndef NSRPAGESLIDER_H_
#define NSRPAGESLIDER_H_

#include "nsrtranslator.h"
#include "nsrreader.h"

#include <bb/cascades/Container>
#include <bb/cascades/Slider>
#include <bb/cascades/TouchEvent>

#if BBNDK_VERSION_AT_LEAST(10,3,1)
#  include <bb/cascades/TrackpadEvent>
#else
typedef bb::cascades::Event TrackpadEvent;

namespace bb { namespace cascades {
	class TrackpadEvent;
}}
#endif

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
	void setEnabled (bool enabled);

Q_SIGNALS:
	void interactionStarted ();
	void currentValueChanged (int page);
	void interactionEnded ();

private Q_SLOTS:
	void onSliderTouchEvent (bb::cascades::TouchEvent *event);
	void onTrackpadEvent (bb::cascades::TrackpadEvent* event);
	void onWantsHighlightChanged (bool wantsHighlight);

private:
	NSRTranslator *			_translator;
	bb::cascades::Slider *		_slider;
	bb::cascades::Container *	_spaceContainer;
	bool				_trackpadActivated;
	float				_originValue;
};

#endif /* NSRPAGESLIDER_H_ */
