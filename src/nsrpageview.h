#ifndef NSRPAGEVIEW_H_
#define NSRPAGEVIEW_H_

#include <QObject>
#include <QSizeF>

#include "nsrrenderedpage.h"

#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>
#include <bb/cascades/ScrollView>
#include <bb/cascades/TextArea>
#include <bb/cascades/TapEvent>
#include <bb/cascades/PinchEvent>

class NSRPageView: public bb::cascades::Container
{
	Q_OBJECT
	Q_ENUMS (NSRViewMode);
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
	bool isInvertedColors () const;
	void setInvertedColors (bool inv);
	QSize getSize () const;
	void setZoomRange (int minZoom, int maxZoom);

Q_SIGNALS:
	void viewTapped ();
	void zoomChanged (int zoom);

private Q_SLOTS:
	void onTappedGesture (bb::cascades::TapEvent *ev);
	void onLayoutFrameChanged (const QRectF& rect);
	void onPinchStarted (bb::cascades::PinchEvent *event);
	void onPinchUpdated (bb::cascades::PinchEvent *event);
	void onPinchEnded (bb::cascades::PinchEvent *event);
	void onPinchCancelled ();

private:
	bb::cascades::ScrollView	*_scrollView;
	bb::cascades::ImageView		*_imageView;
	bb::cascades::TextArea		*_textArea;
	bb::cascades::Container		*_rootContainer;
	bb::cascades::Container		*_textContainer;
	NSRViewMode			_viewMode;
	QSize				_size;
	QSize				_initialScaleSize;
	int				_currentZoom;
	int				_minZoom;
	int				_maxZoom;
	bool				_isInvertedColors;
};

#endif /* NSRPAGEVIEW_H_ */
