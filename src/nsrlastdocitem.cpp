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
	_textView (NULL),
	_label (NULL),
	_viewContainer (NULL),
	_imgTracker (NULL)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ())
						      .background(Color::fromRGBA (0.2, 0.2, 0.2, 1.0));
	rootContainer->setLeftPadding (10);
	rootContainer->setRightPadding (10);
	rootContainer->setTopPadding (10);
	rootContainer->setBottomPadding (10);

	_viewContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(DockLayout::create())
					    .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));

	_imageView = ImageView::create().horizontal(HorizontalAlignment::Fill)
					.vertical(VerticalAlignment::Fill)
					.scalingMethod(ScalingMethod::Fill);
	_textView = Label::create().horizontal(HorizontalAlignment::Fill)
				   .vertical(VerticalAlignment::Fill)
				   .visible(false);
	_textView->textStyle()->setFontSize (FontSize::XXSmall);
	_textView->textStyle()->setColor (Color::Gray);
	_textView->setMultiline (true);

	_viewContainer->setBottomMargin (0);
	_viewContainer->add (_imageView);
	_viewContainer->add (_textView);

	_label = Label::create ().horizontal(HorizontalAlignment::Center)
				 .vertical(VerticalAlignment::Center);
	_label->textStyle()->setColor (Color::fromRGBA (0.8, 0.8, 0.8, 1.0));
	_label->textStyle()->setFontSize (FontSize::XSmall);

	Container *labelContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Fill)
						       .layout(DockLayout::create ())
						       .background(Color::DarkGray);
	labelContainer->setMinHeight (70);
	labelContainer->setTopMargin (0);
	labelContainer->add (_label);

	rootContainer->add (_viewContainer);
	rootContainer->add (labelContainer);

	_imgTracker = new ImageTracker (this);
	connect (_imgTracker, SIGNAL (stateChanged (bb::cascades::ResourceState::Type)),
		 this, SLOT (onImageStateChanged (bb::cascades::ResourceState::Type)));

	setRoot (rootContainer);
}

NSRLastDocItem::~NSRLastDocItem ()
{
}

void
NSRLastDocItem::updateItem (const QString&	title,
			    const QString&	imgPath,
			    const QString&	text,
			    const QString&	path)
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

	if (activate) {
		rootContainer->setBackground (Color::fromRGBA (0, 0.66, 0.87, 1.0));
		_textView->textStyle()->setColor (Color::Black);
	} else {
		rootContainer->setBackground (Color::fromRGBA (0.2, 0.2, 0.2, 1.0));
		_textView->textStyle()->setColor (Color::Gray);
	}
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
	if (state == ResourceState::Loaded)
		_imageView->setImage (_imgTracker->image ());
}



