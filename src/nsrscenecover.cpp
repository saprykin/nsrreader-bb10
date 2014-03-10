#include "nsrscenecover.h"
#include "nsrglobalnotifier.h"

#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/ImagePaint>

#include <bb/system/LocaleHandler>

#include <bb/ImageData>

using namespace bb;
using namespace bb::cascades;

NSRSceneCover::NSRSceneCover (QObject *parent) :
	SceneCover (parent),
	_pageStatus (NULL),
	_titleLabel (NULL),
	_backView (NULL),
	_logoView (NULL),
	_pageView (NULL),
	_textView (NULL),
	_textContainer (NULL),
	_isTextOnly (false),
	_isInvertedColors (false),
	_isEmptyText (false)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .background(Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f))
						      .layout(StackLayout::create());

	_titleContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Top)
					     .layout(DockLayout::create())
					     .background(Color::Black)
					     .visible(false);

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
						       .layout(DockLayout::create())
						       .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));

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

	bool ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
			   this, SLOT (retranslateUi ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);
}

NSRSceneCover::~NSRSceneCover ()
{
}

void
NSRSceneCover::setPageData (const NSRRenderedPage&	page,
                	    const QString&		title,
                	    int				pagesTotal)
{
	if (title.isEmpty () || !page.isValid () || pagesTotal < page.getNumber ())
		return;

	bb::system::LocaleHandler region (bb::system::LocaleType::Region);

	_isEmptyText = page.getText().isEmpty ();
	_pageView->setImage (page.getImage ());
	_textView->setText (_isEmptyText ? trUtf8 ("No text data available for this page") : page.getText ());
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
	_isEmptyText = false;
}

void
NSRSceneCover::updateState (bool isStatic)
{
	_titleContainer->setVisible (!isStatic);
	_backView->setVisible (isStatic);
	_logoView->setVisible (isStatic);
	_pageView->setVisible (!isStatic && !_isTextOnly);
	_textContainer->setVisible (_isTextOnly);
	_pageStatus->setVisible (!isStatic);
	_textContainer->setBackground (_isInvertedColors ? Color::Black : Color::White);
	_textView->textStyle()->setColor (_isInvertedColors ? Color::White : Color::Black);
}

void
NSRSceneCover::retranslateUi ()
{
	if (_isEmptyText && !_textView->text().isEmpty ())
		_textView->setText (trUtf8 ("No text data available for this page"));
}
