#ifndef NSRTOCITEM_H_
#define NSRTOCITEM_H_

#include "nsrtocentry.h"
#include "nsrreader.h"

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListView>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
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

#define NSR_TOC_ITEM_INNER_TAP_PROP	"nsr-toc-item-inner-tap"

class NSRTocItem : public bb::cascades::CustomControl,
		   public bb::cascades::ListItemListener
{
	Q_OBJECT
public:
	NSRTocItem (bb::cascades::Container *parent = 0);
	virtual ~NSRTocItem ();

	/* ListItemListener iface */
	void select (bool select);
	void reset (bool selected, bool activated);
	void activate (bool activate);

	void setTocEntry (const NSRTocEntry *entry);
	void setView (bb::cascades::ListView *view);

private Q_SLOTS:
	void onDynamicDUFactorChanged (float dduFactor);
	void onLocallyFocusedChanged (bool locallyFocused);
	void onImageLocallyFocusedChanged (bool locallyFocused);
	void onImageTouchEvent (bb::cascades::TouchEvent *event);
	void onItemTouchEvent (bb::cascades::TouchEvent *event);
	void onImageTrackpadEvent (bb::cascades::TrackpadEvent* event);
	void onItemTrackpadEvent (bb::cascades::TrackpadEvent* event);

private:
	const NSRTocEntry *			_tocEntry;
	bb::cascades::Label *			_titleLabel;
	bb::cascades::ListView *		_view;
	bb::cascades::ImageView *		_imageView;
	bb::cascades::ImageView *		_imageViewPressed;
	bb::cascades::Container *		_itemContainer;
	bb::cascades::Container *		_labelContainer;
	bb::cascades::Container *		_imageContainer;
	bb::cascades::Container *		_selectContainer;
};

#endif /* NSRTOCITEM_H_ */
