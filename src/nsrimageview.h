#ifndef NSRIMAGEVIEW_H_
#define NSRIMAGEVIEW_H_

#include <QObject>

#include "nsrrenderedpage.h"

#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>
#include <bb/cascades/ScrollView>
#include <bb/cascades/TextArea>

class NSRImageView: public bb::cascades::CustomControl
{
	Q_OBJECT
	Q_ENUMS (NSRViewMode)
public:
	enum NSRViewMode {
		NSR_VIEW_MODE_GRAPHIC	= 0,
		NSR_VIEW_MODE_TEXT	= 1
	};

	NSRImageView (bb::cascades::Container *parent = 0);
	virtual ~NSRImageView ();

	void setPage (const NSRRenderedPage& page);
	void resetPage ();
	void setViewMode (NSRImageView::NSRViewMode mode);
	NSRImageView::NSRViewMode getViewMode () const;
	void setScrollPosition (const QPointF& pos);
	QPointF getScrollPosition () const;

private slots:
	void onWidthChanged (float width);
	void onHeightChanged (float height);

private:
	bb::cascades::ScrollView	*_scrollView;
	bb::cascades::ImageView		*_imageView;
	bb::cascades::TextArea		*_textArea;
	bb::cascades::Container		*_rootContainer;
	NSRViewMode			_viewMode;
};

#endif /* NSRIMAGEVIEW_H_ */
