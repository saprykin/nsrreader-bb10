#include "nsrscenecover.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrreader.h"

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

NSRSceneCover::NSRSceneCover (NSRCoverMode mode, QObject *parent) :
	SceneCover (parent),
	_pageStatus (NULL),
	_titleLabel (NULL),
	_logoView (NULL),
	_pageView (NULL),
	_textView (NULL),
	_textContainer (NULL),
	_mode (mode),
	_textTheme (NSRReadingTheme::Normal),
	_isTextOnly (false),
	_isInvertedColors (false),
	_isEmptyText (false)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .background(Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f))
						      .layout(StackLayout::create());
	if (_mode == NSR_COVER_MODE_FULL) {
		_titleContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						     .vertical(VerticalAlignment::Top)
						     .layout(DockLayout::create())
						     .background(Color::Black)
						     .visible(false);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
		_titleContainer->setLeftPadding (ui()->sddu (1));
		_titleContainer->setRightPadding (ui()->sddu (1));
		_titleContainer->setTopPadding (ui()->sddu (1));
		_titleContainer->setBottomPadding (ui()->sddu (1));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
		_titleContainer->setLeftPadding (ui()->sdu (1));
		_titleContainer->setRightPadding (ui()->sdu (1));
		_titleContainer->setTopPadding (ui()->sdu (1));
		_titleContainer->setBottomPadding (ui()->sdu (1));
#else
		_titleContainer->setLeftPadding (10);
		_titleContainer->setRightPadding (10);
		_titleContainer->setTopPadding (10);
		_titleContainer->setBottomPadding (10);
#endif

		_titleLabel = Label::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Center);
		_titleLabel->textStyle()->setColor (Color::White);
		_titleLabel->textStyle()->setFontSize (FontSize::XSmall);
		_titleContainer->add (_titleLabel);
	}

	Container *imageContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Fill)
						       .layout(DockLayout::create())
						       .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));

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
					    .background(NSRThemeSupport::getReadingBackground (_textTheme))
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
	_textView->textStyle()->setColor (NSRThemeSupport::getReadingColor (_textTheme));
	_textView->textStyle()->setFontSize (FontSize::XSmall);
	_textContainer->add (_textView);

	imageContainer->add (_logoView);
	imageContainer->add (_pageView);
	imageContainer->add (_textContainer);
	imageContainer->add (_pageStatus);

	if (_mode == NSR_COVER_MODE_FULL)
		rootContainer->add (_titleContainer);

	rootContainer->add (imageContainer);

	setContent (rootContainer);

	bool ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
			   this, SLOT (retranslateUi ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	ok = connect (ui (), SIGNAL (dduFactorChanged (float)),
		      this, SLOT (onDynamicDUFactorChanged (float)));
	Q_ASSERT (ok);
#endif
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
	_pageStatus->setStatus (page.getNumber (), pagesTotal);

	if (_mode == NSR_COVER_MODE_FULL) {
		_titleLabel->setText (title);

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
}

void
NSRSceneCover::resetPageData ()
{
	if (_mode == NSR_COVER_MODE_FULL) {
		_titleContainer->setBackground (Color::White);
		_titleLabel->resetText ();
	}
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
	if (_mode == NSR_COVER_MODE_FULL)
		_titleContainer->setVisible (!isStatic);

	_logoView->setVisible (isStatic);
	_pageView->setVisible (!isStatic && !_isTextOnly);
	_textContainer->setVisible (_isTextOnly);
	_pageStatus->setVisible (!isStatic);

	if (!_isInvertedColors) {
		_textContainer->setBackground (NSRThemeSupport::getReadingBackground (_textTheme));
		_textView->textStyle()->setColor (NSRThemeSupport::getReadingColor (_textTheme));
	} else {
		_textContainer->setBackground (Color::Black);
		_textView->textStyle()->setColor (Color::White);
	}
}

void
NSRSceneCover::setInvertedColors (bool invertedColors)
{
	_isInvertedColors = invertedColors;
}

void
NSRSceneCover::setTextTheme (NSRReadingTheme::Type type)
{
	_textTheme = type;
}

void
NSRSceneCover::setFontFamily (const QString& fontFamily)
{
	_textView->textStyle()->setFontFamily (fontFamily);
}

void
NSRSceneCover::setTextOnly (bool textOnly)
{
	_isTextOnly = textOnly;
}

void
NSRSceneCover::retranslateUi ()
{
	if (_isEmptyText && !_textView->text().isEmpty ())
		_textView->setText (trUtf8 ("No text data available for this page"));
}

void
NSRSceneCover::onDynamicDUFactorChanged (float dduFactor)
{
	Q_UNUSED (dduFactor);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	if (_mode == NSR_COVER_MODE_FULL) {
		_titleContainer->setLeftPadding (ui()->sddu (1));
		_titleContainer->setRightPadding (ui()->sddu (1));
		_titleContainer->setTopPadding (ui()->sddu (1));
		_titleContainer->setBottomPadding (ui()->sddu (1));
	}
#endif
}
