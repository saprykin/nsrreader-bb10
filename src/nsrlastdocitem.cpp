#include "nsrlastdocitem.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Color>
#include <bb/cascades/Container>
#include <bb/cascades/ImagePaint>

using namespace bb::cascades;

NSRLastDocItem::NSRLastDocItem (bb::cascades::Container* parent) :
	CustomControl (parent),
	_imageView (NULL),
	_label (NULL)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ())
						      .background(ImagePaint (QUrl ("asset:///border-gray.amd")));

	Container *imgContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(DockLayout::create())
					    .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));

	imgContainer->setLeftPadding (2);
	imgContainer->setRightPadding (3);
	imgContainer->setTopPadding (10);
	imgContainer->setBottomMargin (0);

	_imageView = ImageView::create().horizontal(HorizontalAlignment::Fill)
					.vertical(VerticalAlignment::Fill)
					.scalingMethod(ScalingMethod::Fill)
					.imageSource(QUrl ("asset:///background-black.png"));
	_imageView->setBottomMargin (0);

	imgContainer->add (_imageView);

	_label = Label::create ().horizontal(HorizontalAlignment::Center)
				 .vertical(VerticalAlignment::Center);
	_label->textStyle()->setColor (Color::fromRGBA (0.8, 0.8, 0.8, 1.0));
	_label->textStyle()->setFontSize (FontSize::XSmall);
	_label->setTopMargin (0);

	Container *labelContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Fill)
						       .layout(DockLayout::create ())
						       .background(ImagePaint (QUrl("asset:///background-page-title.amd")));
	labelContainer->setLeftPadding (10);
	labelContainer->setTopMargin (0);
	labelContainer->setMinHeight (70);
	labelContainer->add (_label);

	/* To fit bottom gray title background */
	rootContainer->setLeftPadding (2);
	rootContainer->setRightPadding (3);
	rootContainer->setBottomPadding (2);

	rootContainer->add (imgContainer);
	rootContainer->add (labelContainer);

	setRoot (rootContainer);
}

NSRLastDocItem::~NSRLastDocItem ()
{
}

void
NSRLastDocItem::updateItem (const QString& title, const QString& imgPath)
{
	_label->setText (title);
	_imageView->setImage (Image (imgPath));
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
		rootContainer->setBackground (ImagePaint (QUrl ("asset:///border-gray.amd")));
}


