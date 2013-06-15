#include "nsrlastdocitem.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Color>
#include <bb/cascades/ImagePaint>

#include <QFile>

using namespace bb::cascades;

NSRLastDocItem::NSRLastDocItem (bb::cascades::Container* parent) :
	CustomControl (parent),
	_imageView (NULL),
	_lockImage (NULL),
	_textView (NULL),
	_label (NULL),
	_viewContainer (NULL),
	_imgTracker (NULL)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ())
						      .background(Color::fromRGBA (0.2, 0.2, 0.2, 1.0));

	_viewContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(DockLayout::create())
					    .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));

	_imageView = ImageView::create().horizontal(HorizontalAlignment::Fill)
					.vertical(VerticalAlignment::Fill)
					.scalingMethod(ScalingMethod::AspectFill);
	_textView = Label::create().horizontal(HorizontalAlignment::Fill)
				   .vertical(VerticalAlignment::Fill)
				   .visible(false);
	_textView->textStyle()->setFontSize (FontSize::XXSmall);
	_textView->textStyle()->setColor (Color::Gray);
	_textView->setMultiline (true);

	_viewContainer->add (_imageView);
	_viewContainer->add (_textView);
	_viewContainer->setLeftPadding (3);
	_viewContainer->setRightPadding (3);
	_viewContainer->setTopPadding (3);
	_viewContainer->setBottomPadding (3);

	_label = Label::create ().horizontal(HorizontalAlignment::Center)
				 .vertical(VerticalAlignment::Center);
	_label->textStyle()->setColor (Color::White);
	_label->textStyle()->setFontSize (FontSize::XSmall);

	Container *labelContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Bottom)
						       .layout(DockLayout::create ())
						       .background(Color::Black);
	labelContainer->setOpacity (0.8);
	labelContainer->setMinHeight (70);
	labelContainer->setTopMargin (0);
	labelContainer->setLeftPadding (15);
	labelContainer->setRightPadding (15);
	labelContainer->add (_label);

	_lockImage = ImageView::create().imageSource(QUrl ("asset:///page-lock.png"))
					.horizontal(HorizontalAlignment::Right)
					.vertical(VerticalAlignment::Top)
					.visible(false);

	rootContainer->add (_viewContainer);
	rootContainer->add (labelContainer);
	rootContainer->add (_lockImage);

	_imgTracker = new ImageTracker (this);
	Q_ASSERT (connect (_imgTracker, SIGNAL (stateChanged (bb::cascades::ResourceState::Type)),
			   this, SLOT (onImageStateChanged (bb::cascades::ResourceState::Type))));

	Container *mainContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create())
						      .background(Color::Black);
	mainContainer->setTopPadding (10);
	mainContainer->setRightPadding (10);
	mainContainer->setBottomPadding (10);
	mainContainer->setLeftPadding (10);
	mainContainer->add (rootContainer);

	setRoot (mainContainer);
}

NSRLastDocItem::~NSRLastDocItem ()
{
}

void
NSRLastDocItem::updateItem (const QString&	title,
			    const QString&	imgPath,
			    const QString&	text,
			    const QString&	path,
			    bool		encrypted)
{
	_label->setText (title);
	_path = path;

	if (QFile::exists (imgPath)) {
		_textView->setVisible (false);
		_imageView->setVisible (true);
		_imgTracker->setImageSource (QUrl::fromLocalFile (imgPath));
		_viewContainer->setBottomMargin (0);
	} else {
		_imageView->setVisible (false);
		_textView->setVisible (true);
		_textView->setText (text);
		_viewContainer->setBottomMargin (10);
	}

	_lockImage->setVisible (encrypted);
}

void
NSRLastDocItem::select (bool select)
{
	activate (select);
}

void
NSRLastDocItem::reset (bool selected, bool activated)
{
	Q_UNUSED (activated);

	select (selected);
}

void
NSRLastDocItem::activate (bool activate)
{
	Container *rootContainer = static_cast<Container *> (root ());

	if (activate)
		rootContainer->setBackground (Color::fromRGBA (0, 0.66, 0.87, 1.0));
	else
		rootContainer->setBackground (Color::Black);
}

QString
NSRLastDocItem::getDocumentPath () const
{
	return _path;
}

QString
NSRLastDocItem::getDocumentTitle () const
{
	return _label->text ();
}

void
NSRLastDocItem::onImageStateChanged (bb::cascades::ResourceState::Type state)
{
	if (state == ResourceState::Loaded) {
		_imageView->setImage (_imgTracker->image ());

		if ((double) _imgTracker->height () / _imgTracker->width () < 1.2) {
			_imageView->setScalingMethod (ScalingMethod::AspectFit);
			_imageView->setVerticalAlignment (VerticalAlignment::Fill);
			_imageView->setHorizontalAlignment (HorizontalAlignment::Fill);
		} else {
			_imageView->setScalingMethod (ScalingMethod::AspectFill);
			_imageView->setVerticalAlignment (VerticalAlignment::Fill);
			_imageView->setHorizontalAlignment (HorizontalAlignment::Fill);
		}
	}
}
