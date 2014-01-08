#include "nsrscenecover.h"

#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/ImagePaint>

#include <bb/system/LocaleHandler>

#include <bb/device/HardwareInfo>

#include <bb/ImageData>

using namespace bb;
using namespace bb::cascades;
using namespace bb::device;

NSRSceneCover::NSRSceneCover (QObject *parent) :
	SceneCover (parent),
	_pageStatus (NULL),
	_titleLabel (NULL),
	_backView (NULL),
	_logoView (NULL),
	_pageView (NULL),
	_textView (NULL),
	_textContainer (NULL),
	_isTextOnly (false)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .background(Color::fromRGBA (0.95f, 0.95f, 0.95f, 1.0f))
						      .layout(StackLayout::create());

	_titleContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Top)
					     .layout(DockLayout::create())
					     .background(Color::Black)
					     .visible(false);

	HardwareInfo hardwareInfo (this);

	if (hardwareInfo.isPhysicalKeyboardDevice ()) {
		_titleContainer->setMinHeight (56);
		_titleContainer->setMaxHeight (56);
	} else {
		_titleContainer->setMinHeight (62);
		_titleContainer->setMaxHeight (62);
	}

	_titleContainer->setLeftPadding (10);
	_titleContainer->setRightPadding (10);
	_titleContainer->setTopPadding (10);
	_titleContainer->setBottomPadding (10);

	_titleLabel = Label::create().horizontal(HorizontalAlignment::Center)
				     .vertical(VerticalAlignment::Center);
	_titleLabel->textStyle()->setFontSize (FontSize::XSmall);
	_titleContainer->add (_titleLabel);

	Container *imageContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Fill)
						       .layout(DockLayout::create());

	_backView = ImageView::create().horizontal(HorizontalAlignment::Fill)
				       .vertical(VerticalAlignment::Fill)
				       .scalingMethod(ScalingMethod::AspectFill);
	_backView->setImageSource (QUrl ("asset:///active-frame-background.png"));

	_logoView = ImageView::create().horizontal(HorizontalAlignment::Center)
				       .vertical(VerticalAlignment::Center);
	_logoView->setImageSource (QUrl ("asset:///active-frame-logo.png"));

	_pageView = ImageView::create().horizontal(HorizontalAlignment::Fill)
				       .vertical(VerticalAlignment::Fill)
				       .scalingMethod(ScalingMethod::AspectFill)
				       .visible(false);

	_pageStatus = new NSRPageStatus ();
	_pageStatus->setAutoHide (false);
	_pageStatus->setStatusBackground (Color::Black);
	_pageStatus->setStatusBackgroundOpacity (0.5f);
	_pageStatus->setFontSize (FontSize::XXSmall);
	_pageStatus->setHorizontalAlignment (HorizontalAlignment::Left);
	_pageStatus->setVerticalAlignment (VerticalAlignment::Top);

	_textContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(StackLayout::create())
					    .background(Color::White)
					    .visible(false);
	_textView = TextArea::create().horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Fill)
				      .editable(false)
				      .inputFlags(TextInputFlag::SpellCheckOff |
						  TextInputFlag::PredictionOff |
						  TextInputFlag::AutoCapitalizationOff |
						  TextInputFlag::AutoCorrectionOff |
						  TextInputFlag::AutoPeriodOff |
						  TextInputFlag::WordSubstitutionOff |
						  TextInputFlag::VirtualKeyboardOff)
				      .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));
	_textView->setTopPadding (0);
	_textView->setBottomPadding (0);
	_textView->setTextFormat (TextFormat::Plain);
	_textView->textStyle()->setColor (Color::Black);
	_textView->textStyle()->setFontSize (FontSize::XSmall);
	_textContainer->add (_textView);

	imageContainer->add (_backView);
	imageContainer->add (_logoView);
	imageContainer->add (_pageView);
	imageContainer->add (_textContainer);
	imageContainer->add (_pageStatus);

	rootContainer->add (_titleContainer);
	rootContainer->add (imageContainer);

	setContent (rootContainer);
}

NSRSceneCover::~NSRSceneCover ()
{
}

void
NSRSceneCover::setPageData (const NSRRenderedPage&	page,
                	    const QString&		title,
                	    int				pagesTotal)
{
	if (page.isEmpty () || title.isEmpty () || page.getNumber () < 1 || pagesTotal < page.getNumber ())
		return;

	bb::system::LocaleHandler region (bb::system::LocaleType::Region);

	_pageView->setImage (page.getImage ());
	_textView->setText (page.getText ());
	_titleLabel->setText (title);
	_pageStatus->setStatus (page.getNumber (), pagesTotal);

	QString	extension = QFileInfo(title).suffix().toLower ();
	QString background;

	if (extension == "pdf")
		background = "asset:///pdf-header.png";
	else if (extension == "djvu" || extension == "djv")
		background = "asset:///djvu-header.png";
	else if (extension == "tiff" || extension == "tif")
		background = "asset:///tiff-header.png";
	else
		background = "asset:///txt-header.png";

	_titleContainer->setBackground (ImagePaint (Image (QUrl (background)), RepeatPattern::Fill));
	_isTextOnly = !page.getImage().isValid () && !page.getText().isEmpty ();
}

void
NSRSceneCover::resetPageData ()
{
	_titleContainer->setBackground (Color::White);
	_titleLabel->resetText ();
	_pageView->resetImage ();
	_pageView->resetImageSource ();
	_pageStatus->resetStatus ();
	_textView->resetText ();
	_isTextOnly = false;
}

void
NSRSceneCover::setStatic (bool isStatic)
{
	_titleContainer->setVisible (!isStatic);
	_backView->setVisible (isStatic);
	_logoView->setVisible (isStatic);
	_pageView->setVisible (!isStatic && !_isTextOnly);
	_textContainer->setVisible (_isTextOnly);
	_pageStatus->setVisible (!isStatic);
}
