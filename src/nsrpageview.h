#ifndef NSRPAGEVIEW_H_
#define NSRPAGEVIEW_H_

#include <QObject>
#include <QSizeF>

#include "nsrrenderedpage.h"

#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>
#include <bb/cascades/ScrollView>
#include <bb/cascades/TextArea>
#include <bb/cascades/TapEvent>

class NSRPageView: public bb::cascades::CustomControl
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
	bool isInvertedColors () const;
	void setInvertedColors (bool inv);
	QSize getSize () const;

Q_SIGNALS:
	void viewTapped ();

private Q_SLOTS:
	void onWidthChanged (float width);
	void onHeightChanged (float height);
	void onTappedGesture (bb::cascades::TapEvent *ev);
	void onLayoutFrameChanged (const QRectF& rect);

private:
	bb::cascades::ScrollView	*_scrollView;
	bb::cascades::ImageView		*_imageView;
	bb::cascades::TextArea		*_textArea;
	bb::cascades::Container		*_rootContainer;
	bb::cascades::Container		*_textContainer;
	NSRViewMode			_viewMode;
	QSize				_size;
	bool				_isInvertedColors;
};

#endif /* NSRPAGEVIEW_H_ */
