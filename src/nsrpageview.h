#ifndef NSRPAGEVIEW_H_
#define NSRPAGEVIEW_H_

#include <QObject>
#include <QSizeF>
#include <QTime>
#include <QTimerEvent>

#include "nsrrenderedpage.h"

#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>
#include <bb/cascades/ScrollView>
#include <bb/cascades/TextArea>
#include <bb/cascades/TapEvent>
#include <bb/cascades/DoubleTapEvent>
#include <bb/cascades/PinchEvent>

class NSRPageView: public bb::cascades::Container
{
	Q_OBJECT
	Q_ENUMS (NSRViewMode)
public:
	enum NSRViewMode {
		NSR_VIEW_MODE_GRAPHIC	= 0,
		NSR_VIEW_MODE_TEXT	= 1,
		NSR_VIEW_MODE_PREFERRED	= 2
	};

	NSRPageView (bb::cascades::Container *parent = 0);
	virtual ~NSRPageView ();

	void setPage (const NSRRenderedPage& page);
	void resetPage ();
	void setViewMode (NSRPageView::NSRViewMode mode);
	NSRPageView::NSRViewMode getViewMode () const;
	void setScrollPosition (const QPointF& pos);
	QPointF getScrollPosition () const;
	void setScrollPositionOnLoad (const QPointF& pos);
	bool isInvertedColors () const;
	void setInvertedColors (bool inv);
	QSize getSize () const;
	void setZoomRange (double minZoom, double maxZoom);
	void fitToWidth ();
	int getTextZoom () const;
	void setTextZoom (int fontSize);
	void setZoomEnabled (bool enabled);
	bool isZoomEnabled () const;

Q_SIGNALS:
	void viewTapped ();
	void zoomChanged (double zoom, bool toWidth);
	void nextPageRequested ();
	void prevPageRequested ();
	void fitToWidthRequested ();

protected:
	void timerEvent (QTimerEvent *ev);

private Q_SLOTS:
	void onTapGesture (bb::cascades::TapEvent *ev);
	void onDoubleTappedGesture (bb::cascades::DoubleTapEvent *ev);
	void onLayoutFrameChanged (const QRectF& rect);
	void onPinchStarted (bb::cascades::PinchEvent *event);
	void onPinchUpdated (bb::cascades::PinchEvent *event);
	void onPinchEnded (bb::cascades::PinchEvent *event);
	void onPinchCancelled ();

private:
	bb::cascades::ScrollView	*_scrollView;
	bb::cascades::ScrollView	*_textScrollView;
	bb::cascades::ImageView		*_imageView;
	bb::cascades::TextArea		*_textArea;
	bb::cascades::Container		*_rootContainer;
	bb::cascades::Container		*_textContainer;
	NSRViewMode			_viewMode;
	QSize				_size;
	QSize				_initialScaleSize;
	QPointF				_delayedScrollPos;
	QTime				_lastTapTime;
	double				_currentZoom;
	double				_minZoom;
	double				_maxZoom;
	int				_lastTapTimer;
	int				_initialFontSize;
	bool				_isInvertedColors;
	bool				_useDelayedScroll;
	bool				_isZooming;
	bool				_isZoomingEnabled;
};

#endif /* NSRPAGEVIEW_H_ */
