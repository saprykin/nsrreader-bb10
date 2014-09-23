#ifndef NSRPAGEVIEW_H_
#define NSRPAGEVIEW_H_

#include "nsrrenderedpage.h"
#include "nsrtranslator.h"
#include "nsrabstractdocument.h"

#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>
#include <bb/cascades/ScrollView>
#include <bb/cascades/TextArea>
#include <bb/cascades/TapEvent>
#include <bb/cascades/DoubleTapEvent>
#include <bb/cascades/PinchEvent>
#include <bb/cascades/ActionSet>

#include <QObject>
#include <QSizeF>
#include <QTime>
#include <QTimerEvent>

class NSRPageView: public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRPageView (bb::cascades::Container *parent = 0);
	virtual ~NSRPageView ();

	void setPage (const NSRRenderedPage& page);
	void resetPage ();
	void setViewMode (NSRAbstractDocument::NSRDocumentStyle mode);
	NSRAbstractDocument::NSRDocumentStyle getViewMode () const;
	void setScrollPosition (const QPointF& pos, NSRAbstractDocument::NSRDocumentStyle mode);
	QPointF getScrollPosition (NSRAbstractDocument::NSRDocumentStyle mode) const;
	void setScrollPositionOnLoad (const QPointF& pos, NSRAbstractDocument::NSRDocumentStyle mode);
	bool isInvertedColors () const;
	void setInvertedColors (bool inv);
	QSize getSize () const;
	void setZoomRange (double minZoom, double maxZoom);
	void fitToWidth (NSRRenderRequest::NSRRenderReason reason);
	int getTextZoom () const;
	void setTextZoom (int fontSize);
	void setZoomEnabled (bool enabled);
	bool isZoomEnabled () const;
	void setActionsEnabled (bool enabled);
	void setGesturesEnabled (bool enabled);
	inline bool isActionsEnabled () const {
		return _isActionsEnabled;
	}
	inline bool isGesturesEnabled () const {
		return _isGesturesEnabled;
	}

public Q_SLOTS:
	void zoomIn ();
	void zoomOut ();

Q_SIGNALS:
	void viewTapped ();
	void zoomChanged (double zoom, NSRRenderRequest::NSRRenderReason reason);
	void sizeChanged (const QSize& size);
	void nextPageRequested ();
	void prevPageRequested ();
	void fitToWidthRequested ();
	void rotateLeftRequested ();
	void rotateRightRequested ();

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
	void retranslateUi ();

private:
	void retranslateTitle ();
	bool checkGestureForNavigation (const bb::cascades::AbstractGestureEvent *event);

	NSRRenderedPage				_page;
	NSRTranslator				*_translator;
	bb::cascades::ScrollView		*_scrollView;
	bb::cascades::ScrollView		*_textScrollView;
	bb::cascades::ImageView			*_imageView;
	bb::cascades::TextArea			*_textArea;
	bb::cascades::Container			*_rootContainer;
	bb::cascades::Container			*_textContainer;
	bb::cascades::Container			*_imageContainer;
	bb::cascades::ActionSet			*_actionSet;
	NSRAbstractDocument::NSRDocumentStyle	_viewMode;
	QSize					_size;
	QSize					_initialScaleSize;
	QPointF					_delayedScrollPos;
	QPointF					_delayedTextScrollPos;
	QPointF					_initialScalePos;
	QTime					_lastTapTime;
	double					_currentZoom;
	double					_minZoom;
	double					_maxZoom;
	int					_lastTapTimer;
	int					_initialFontSize;
	bool					_isInvertedColors;
	bool					_isZooming;
	bool					_isZoomingEnabled;
	bool					_isActionsEnabled;
	bool					_isGesturesEnabled;
};

#endif /* NSRPAGEVIEW_H_ */
