#include "nsrtocitem.h"
#include "nsrthemesupport.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Color>
#include <bb/cascades/Divider>

#if BBNDK_VERSION_AT_LEAST(10,3,1)
#include <bb/cascades/TrackpadHandler>
#endif

using namespace bb::cascades;

NSRTocItem::NSRTocItem (bb::cascades::Container* parent) :
	CustomControl (parent),
	_tocEntry (NULL),
	_titleLabel (NULL),
	_view (NULL),
	_imageView (NULL),
	_imageViewPressed (NULL),
	_itemContainer (NULL),
	_labelContainer (NULL),
	_imageContainer (NULL)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create().orientation(LayoutOrientation::TopToBottom))
						      .background(Color::Transparent);

	Container *innerRootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
								      .vertical(VerticalAlignment::Fill)
								      .layout(DockLayout::create ());

	_itemContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Center)
					    .layout(StackLayout::create().orientation (LayoutOrientation::LeftToRight));

	_labelContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Center)
					     .layoutProperties(StackLayoutProperties::create().spaceQuota (1.0));

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_itemContainer->setLeftPadding (ui()->sddu (2));
	_labelContainer->setLeftMargin (ui()->sddu (2));
	_labelContainer->setTopPadding (ui()->sddu (2.3));
	_labelContainer->setBottomPadding (ui()->sddu (2.3));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_itemContainer->setLeftPadding (ui()->sdu (2));
	_labelContainer->setLeftMargin (ui()->ddu (2));
	_labelContainer->setTopPadding (ui()->sdu (2.3));
	_labelContainer->setBottomPadding (ui()->sdu (2.3));
#else
	_itemContainer->setLeftPadding (20);
	_labelContainer->setLeftMargin (20);
	_labelContainer->setTopPadding (23);
	_labelContainer->setBottomPadding (23);
#endif

	_titleLabel = Label::create("").horizontal(HorizontalAlignment::Fill)
				       .vertical(VerticalAlignment::Fill)
				       .topMargin(0.0f)
				       .bottomMargin(0.0f);
	_titleLabel->textStyle()->setFontSize (FontSize::Large);

	_imageContainer = Container::create().horizontal(HorizontalAlignment::Right)
					     .vertical(VerticalAlignment::Bottom)
					     .layout(DockLayout::create ());

	_imageView = ImageView::create ();
	_imageViewPressed = ImageView::create ();

	QString themeDir = NSRThemeSupport::instance()->getAssetsThemeDirectory ();

	_imageView->setImageSource (QUrl (QString("asset:///%1/list-item-expand.png").arg (themeDir)));
	_imageViewPressed->setImageSource (QUrl (QString("asset:///%1/list-item-expand-pressed.png").arg (themeDir)));
	_imageViewPressed->setVisible (false);

	_labelContainer->add (_titleLabel);
	_imageContainer->add (_imageView);
	_imageContainer->add (_imageViewPressed);

	_itemContainer->add (_labelContainer);
	_itemContainer->add (_imageContainer);

	innerRootContainer->add (_itemContainer);

	rootContainer->add (innerRootContainer);
	rootContainer->add (Divider::create().bottomMargin(0.0f).topMargin(0.0f));

	setRoot (rootContainer);

	bool ok = connect (_imageContainer, SIGNAL (touch (bb::cascades::TouchEvent *)),
			   this, SLOT (onImageTouchEvent (bb::cascades::TouchEvent *)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_labelContainer, SIGNAL (touch (bb::cascades::TouchEvent *)),
		      this, SLOT (onItemTouchEvent (bb::cascades::TouchEvent *)));
	Q_ASSERT (ok);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	navigation()->setLeftAllowed (false);
	navigation()->setRight (_imageContainer);

	_imageContainer->navigation()->setDefaultHighlightEnabled (false);
	_imageContainer->navigation()->setRightAllowed (false);
	_imageContainer->navigation()->setLeft (this);

	ok = connect (ui (), SIGNAL (dduFactorChanged (float)),
		      this, SLOT (onDynamicDUFactorChanged (float)));
	Q_ASSERT (ok);

	ok = connect (this, SIGNAL (locallyFocusedChanged (bool)),
		      this, SLOT (onLocallyFocusedChanged (bool)));
	Q_ASSERT (ok);

	ok = connect (_imageContainer, SIGNAL (locallyFocusedChanged (bool)),
		      this, SLOT (onImageLocallyFocusedChanged (bool)));
	Q_ASSERT (ok);

	TrackpadHandler *trackpadHandler = TrackpadHandler::create()
						.onTrackpad (this, SLOT (onImageTrackpadEvent (bb::cascades::TrackpadEvent *)));

	_imageContainer->addEventHandler (trackpadHandler);
#endif
}

NSRTocItem::~NSRTocItem ()
{
}

void
NSRTocItem::select (bool select)
{
	if (select)
		(static_cast<Container *> (root ()))->setBackground (NSRThemeSupport::instance()->getListSelection ());
	else
		(static_cast<Container *> (root ()))->setBackground (Color::Transparent);
}

void
NSRTocItem::reset (bool selected, bool activated)
{
	Q_UNUSED (activated);

	select (selected);
}

void
NSRTocItem::activate (bool activate)
{
	select (activate);
}

void
NSRTocItem::setTocEntry (const NSRTocEntry *entry)
{
	if (_tocEntry == entry)
		return;

	_tocEntry = entry;
	_titleLabel->setText (_tocEntry != NULL ? _tocEntry->getTitle () : QString ());
	_imageContainer->setVisible (_tocEntry != NULL && _tocEntry->getChildren().count () > 0);
}

void
NSRTocItem::setView (bb::cascades::ListView *view)
{
	_view = view;
}

void
NSRTocItem::onDynamicDUFactorChanged (float dduFactor)
{
	Q_UNUSED (dduFactor);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_itemContainer->setLeftPadding (ui()->sddu (2));
	_labelContainer->setLeftMargin (ui()->sddu (2));
	_labelContainer->setTopPadding (ui()->sddu (2.3));
	_labelContainer->setBottomPadding (ui()->sddu (2.3));
#endif
}

void
NSRTocItem::onLocallyFocusedChanged (bool locallyFocused)
{
#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_imageContainer->navigation()->setFocusPolicy (locallyFocused ? NavigationFocusPolicy::Focusable :
									NavigationFocusPolicy::NotFocusable);

#else
	Q_UNUSED (lcoalllocallyFocused);
#endif
}

void
NSRTocItem::onImageLocallyFocusedChanged (bool locallyFocused)
{
#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_imageView->setVisible (locallyFocused);
	_imageViewPressed->setVisible (!locallyFocused);
#else
	Q_UNUSED (lcoalllocallyFocused);
#endif
}

void
NSRTocItem::onImageTouchEvent (bb::cascades::TouchEvent *event)
{
	if (event->isDown ()) {
		_imageView->setVisible (false);
		_imageViewPressed->setVisible (true);
		_view->setProperty (NSR_TOC_ITEM_INNER_TAP_PROP, true);
	} else if (event->isCancel () || event->isUp ()) {
		_imageView->setVisible (true);
		_imageViewPressed->setVisible (false);
	}
}

void
NSRTocItem::onItemTouchEvent (bb::cascades::TouchEvent *event)
{
	if (event->isDown ())
		_view->setProperty (NSR_TOC_ITEM_INNER_TAP_PROP, false);
}

void
NSRTocItem::onImageTrackpadEvent (bb::cascades::TrackpadEvent* event)
{
#if BBNDK_VERSION_AT_LEAST(10,3,1)
	if (event->trackpadEventType () == TrackpadEventType::Press) {
		_imageView->setVisible (false);
		_imageViewPressed->setVisible (true);
		_view->setProperty (NSR_TOC_ITEM_INNER_TAP_PROP, true);
	} else if (event->trackpadEventType () == TrackpadEventType::Cancel ||
		   event->trackpadEventType () == TrackpadEventType::Release) {
		_imageView->setVisible (true);
		_imageViewPressed->setVisible (false);
	}
#else
	Q_UNUSED (event);
#endif
}

void
NSRTocItem::onItemTrackpadEvent (bb::cascades::TrackpadEvent* event)
{
#if BBNDK_VERSION_AT_LEAST(10,3,1)
	if (event->trackpadEventType () == TrackpadEventType::Press)
		_view->setProperty (NSR_TOC_ITEM_INNER_TAP_PROP, false);
#else
	Q_UNUSED (event);
#endif
}
