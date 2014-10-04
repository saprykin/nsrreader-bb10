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

	inline NSRAbstractDocument::NSRDocumentStyle getViewMode () const {
		return _viewMode;
	}

	inline QSize getSize () const {
		return _size;
	}

	QPointF getScrollPosition (NSRAbstractDocument::NSRDocumentStyle mode) const;

	double getZoom () const;
	int getTextZoom () const;

	inline bool isZoomEnabled () const {
		return _isZoomingEnabled;
	}

	inline bool isActionsEnabled () const {
		return _isActionsEnabled;
	}

	inline bool isGesturesEnabled () const {
		return _isGesturesEnabled;
	}

	inline bool isInvertedColors () const {
		return _isInvertedColors;
	}

	bool isOverzoom () const;

	void setPage (const NSRRenderedPage& page);
	void setViewMode (NSRAbstractDocument::NSRDocumentStyle mode);
	void setScrollPosition (const QPointF& pos, NSRAbstractDocument::NSRDocumentStyle mode);
	void setScrollPositionOnLoad (const QPointF& pos, NSRAbstractDocument::NSRDocumentStyle mode);

	inline void setMaxZoom (double maxZoom) {
		_maxZoom = maxZoom;
	}

	void setTextZoom (int fontSize);

	inline void setZoomEnabled (bool enabled) {
		_isZoomingEnabled = enabled;
	}

	void setActionsEnabled (bool enabled);
	void setGesturesEnabled (bool enabled);
	void setInvertedColors (bool inv);

	void fitToWidth (NSRRenderRequest::NSRRenderReason reason);
	void resetPage ();
	void resetOverzoom ();

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
	double getOutscale (const NSRRenderedPage& page) const;
	void rescaleImage (const QSizeF& size, const QPointF& center);

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
