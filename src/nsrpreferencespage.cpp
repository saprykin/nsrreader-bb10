#include "nsrpreferencespage.h"
#include "nsrsettings.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrhardwareinfo.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Divider>
#include <bb/cascades/Header>

using namespace bb::cascades;

NSRPreferencesPage::NSRPreferencesPage (QObject *parent) :
	Page (parent),
	_translator (NULL),
	_themeContainer (NULL),
	_textThemeContainer (NULL),
	_fullscreenContainer (NULL),
	_cropContainer (NULL),
	_screenLockContainer (NULL),
	_encodingContainer (NULL),
	_fontExampleContainer (NULL),
	_isFullscreen (NULL),
	_isAutoCrop (NULL),
	_isPreventScreenLock (NULL),
	_isEncodingAutodetection (NULL),
	_isBrandColors (NULL),
	_encodingsList (NULL),
	_themeList (NULL),
	_textThemeList (NULL),
	_fontList (NULL),
	_fontExampleLabel (NULL)
{
	QString defEncoding ("UTF-8");

	_translator = new NSRTranslator (this);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());

	_isFullscreen = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isAutoCrop = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isPreventScreenLock = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isEncodingAutodetection = ToggleButton::create().horizontal(HorizontalAlignment::Right);
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_isBrandColors = ToggleButton::create().horizontal(HorizontalAlignment::Right);
#endif
	_encodingsList = DropDown::create().horizontal(HorizontalAlignment::Fill);
	_themeList = DropDown::create().horizontal(HorizontalAlignment::Fill);
	_textThemeList = DropDown::create().horizontal(HorizontalAlignment::Fill);

	_themeList->setFocusPolicy (FocusPolicy::Touch);
	_textThemeList->setFocusPolicy (FocusPolicy::Touch);

	_isFullscreen->setChecked (NSRSettings::instance()->isFullscreenMode ());
	_isAutoCrop->setChecked (NSRSettings::instance()->isAutoCrop ());
	_isPreventScreenLock->setChecked (NSRSettings::instance()->isPreventScreenLock ());
	_isEncodingAutodetection->setChecked (NSRSettings::instance()->isEncodingAutodetection ());
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_isBrandColors->setChecked (NSRSettings::instance()->isBrandColors ());
#endif
	_encodingsList->setEnabled (!_isEncodingAutodetection->isChecked ());
	_encodingsList->setFocusPolicy (FocusPolicy::Touch);

	bool ok = connect (_isEncodingAutodetection, SIGNAL (checkedChanged (bool)),
			   this, SLOT (onEncodingAutodetectionCheckedChanged (bool)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	QString textEncoding = NSRSettings::instance()->getTextEncoding ();
	QStringList encodings = NSRSettings::getSupportedEncodings ();
	int encodingIndex = NSRSettings::mapEncodingToIndex (textEncoding);

	int count = encodings.count ();
	for (int i = 0; i < count; ++i)
		_encodingsList->add (Option::create().text(encodings.at (i)));
	_encodingsList->setSelectedIndex (encodingIndex);

	Option *optionThemeBright = Option::create().text(trUtf8 ("Bright", "Bright UI theme"));
	Option *optionThemeDark = Option::create().text(trUtf8 ("Dark", "Dark UI theme"));

	_themeList->setTitle (trUtf8 ("Visual Theme", "Visual theme settings"));
	_themeList->add (optionThemeBright);
	_themeList->add (optionThemeDark);

	if (NSRHardwareInfo::instance()->isOLED ()) {
		_themeList->setSelectedIndex (VisualStyle::Dark - 1);
		_themeList->setEnabled (false);
	} else
		_themeList->setSelectedIndex (NSRSettings::instance()->getVisualStyle () - 1);

	Option *optionNormalText = Option::create().text(trUtf8 ("Normal", "Normal (black & white) text color theme"));
	Option *optionSepiaText = Option::create().text(trUtf8 ("Sepia", "Sepia text color theme"));

	_textThemeList->setTitle (trUtf8 ("Text Theme", "Text color theme"));
	_textThemeList->add (optionNormalText);
	_textThemeList->add (optionSepiaText);
	_textThemeList->setSelectedIndex (NSRSettings::instance()->getTextTheme () - 1);

	/* 'Visual Theme' section */
	_themeContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .layout(StackLayout::create ());

	Label *themeInfoLabel = Label::create(trUtf8 ("Close and reopen the app to apply changes."))
				      .horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center)
				      .multiline(true);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	Container *themeInContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						         .layout(StackLayout::create()
								 	     .orientation(LayoutOrientation::LeftToRight));

	Label *brandColorsLabel = Label::create(trUtf8 ("Brand Colors", "Option in preferences"))
				        .horizontal(HorizontalAlignment::Left)
				        .vertical(VerticalAlignment::Center)
				        .multiline(true)
				        .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	themeInContainer->add (brandColorsLabel);
	themeInContainer->add (_isBrandColors);
#endif

	themeInfoLabel->textStyle()->setFontSize (FontSize::XSmall);
	themeInfoLabel->textStyle()->setColor (NSRThemeSupport::instance()->getTipText ());

	_themeContainer->add (_themeList);
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_themeContainer->add (themeInContainer);
#endif
	_themeContainer->add (themeInfoLabel);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_themeContainer->setTopPadding (ui()->sddu (2));
	_themeContainer->setLeftPadding (ui()->sddu (2));
	_themeContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_themeContainer->setTopPadding (ui()->sdu (2));
	_themeContainer->setLeftPadding (ui()->sdu (2));
	_themeContainer->setRightPadding (ui()->sdu (2));
#else
	_themeContainer->setTopPadding (20);
	_themeContainer->setLeftPadding (20);
	_themeContainer->setRightPadding (20);
#endif

	/* 'Text' section */
	_textThemeContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						 .layout(StackLayout::create ());

	_fontList = DropDown::create().title(trUtf8 ("Font"))
				      .horizontal(HorizontalAlignment::Fill);
	_fontList->setFocusPolicy (FocusPolicy::Touch);

	QStringList fontFamilies   = NSRSettings::getSupportedFontFamilies ();
	QString defaultFontFamily  = NSRSettings::getDefaultFontFamily ();
	QString savedFontFamily    = NSRSettings::instance()->getFontFamily ();
	Option *selectedFontOption = NULL;

	foreach (QString family, fontFamilies) {
		Option *familyOption = Option::create().text (family);

		if (family == savedFontFamily ||
		    (family == defaultFontFamily && selectedFontOption == NULL))
			selectedFontOption = familyOption;

		_fontList->add (familyOption);
	}

	_fontExampleContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						   .layout(StackLayout::create ());
	_fontExampleLabel = Label::create(trUtf8 ("Example text. Text theme and font are used "
						  "only for plain text files and reflow mode."))
				  .horizontal(HorizontalAlignment::Fill)
				  .vertical(VerticalAlignment::Center)
				  .multiline(true);
	_fontExampleContainer->add (_fontExampleLabel);

	_textThemeContainer->add (_textThemeList);
	_textThemeContainer->add (_fontList);
	_textThemeContainer->add (_fontExampleContainer);

	ok = connect (_fontList, SIGNAL (selectedOptionChanged (bb::cascades::Option *)),
		      this, SLOT (onSelectedFontOptionChanged (bb::cascades::Option *)));
	Q_ASSERT (ok);

	ok = connect (_textThemeList, SIGNAL (selectedOptionChanged (bb::cascades::Option *)),
		      this, SLOT (onSelectedTextThemeOptionChanged (bb::cascades::Option *)));
	Q_ASSERT (ok);

	if (selectedFontOption != NULL)
		_fontList->setSelectedOption (selectedFontOption);

	onSelectedTextThemeOptionChanged (_textThemeList->selectedOption ());

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_textThemeContainer->setTopPadding (ui()->sddu (2));
	_textThemeContainer->setLeftPadding (ui()->sddu (2));
	_textThemeContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_textThemeContainer->setTopPadding (ui()->sdu (2));
	_textThemeContainer->setLeftPadding (ui()->sdu (2));
	_textThemeContainer->setRightPadding (ui()->sdu (2));
#else
	_textThemeContainer->setTopPadding (20);
	_textThemeContainer->setLeftPadding (20);
	_textThemeContainer->setRightPadding (20);
#endif

	/* 'Fullscreen Mode' option */
	_fullscreenContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						  .layout(StackLayout::create()
								      .orientation(LayoutOrientation::LeftToRight));

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_fullscreenContainer->setTopPadding (ui()->sddu (2));
	_fullscreenContainer->setLeftPadding (ui()->sddu (2));
	_fullscreenContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullscreenContainer->setTopPadding (ui()->sdu (2));
	_fullscreenContainer->setLeftPadding (ui()->sdu (2));
	_fullscreenContainer->setRightPadding (ui()->sdu (2));
#else
	_fullscreenContainer->setTopPadding (20);
	_fullscreenContainer->setLeftPadding (20);
	_fullscreenContainer->setRightPadding (20);
#endif

	Label *fullscreenLabel = Label::create(trUtf8 ("Fullscreen Mode", "Option in preferences"))
				       .horizontal(HorizontalAlignment::Left)
				       .vertical(VerticalAlignment::Center)
				       .multiline(true)
				       .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	_fullscreenContainer->add (fullscreenLabel);
	_fullscreenContainer->add (_isFullscreen);

	/* 'Crop Blank Edges' option */
	_cropContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .layout(StackLayout::create()
								.orientation(LayoutOrientation::LeftToRight));

	Label *cropLabel = Label::create(trUtf8 ("Crop Blank Edges", "Option in preferences"))
				 .horizontal(HorizontalAlignment::Left)
				 .vertical(VerticalAlignment::Center)
				 .multiline(true)
				 .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	_cropContainer->add (cropLabel);
	_cropContainer->add (_isAutoCrop);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_cropContainer->setLeftPadding (ui()->sddu (2));
	_cropContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_cropContainer->setLeftPadding (ui()->sdu (2));
	_cropContainer->setRightPadding (ui()->sdu (2));
#else
	_cropContainer->setLeftPadding (20);
	_cropContainer->setRightPadding (20);
#endif

	/* 'Prevent Screen Locking' option */
	Container *screenLockInContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						              .layout(StackLayout::create()
										  .orientation(LayoutOrientation::LeftToRight));

	Label *preventScreenLockLabel = Label::create(trUtf8 ("Prevent Screen Locking", "Option in preferences"))
					      .horizontal(HorizontalAlignment::Left)
					      .vertical(VerticalAlignment::Center)
					      .multiline(true)
					      .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	Label *screenLockInfo = Label::create(trUtf8 ("Do not automatically lock screen while reading."))
				      .horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center);
	screenLockInfo->textStyle()->setFontSize (FontSize::XSmall);
	screenLockInfo->textStyle()->setColor (NSRThemeSupport::instance()->getTipText ());
	screenLockInfo->setMultiline (true);

	screenLockInContainer->add (preventScreenLockLabel);
	screenLockInContainer->add (_isPreventScreenLock);

	_screenLockContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						  .layout(StackLayout::create ());

	_screenLockContainer->add (screenLockInContainer);
	_screenLockContainer->add (screenLockInfo);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_screenLockContainer->setLeftPadding (ui()->sddu (2));
	_screenLockContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_screenLockContainer->setLeftPadding (ui()->sdu (2));
	_screenLockContainer->setRightPadding (ui()->sdu (2));
#else
	_screenLockContainer->setLeftPadding (20);
	_screenLockContainer->setRightPadding (20);
#endif

	/* 'Text Encoding' section */
	Container *encodingInContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						            .layout(StackLayout::create()
										.orientation(LayoutOrientation::LeftToRight));

	Label *encodingAutodetectLabel = Label::create(trUtf8 ("Autodetect Encoding", "Option in preferences"))
					       .horizontal(HorizontalAlignment::Left)
					       .vertical(VerticalAlignment::Center)
					       .multiline(true)
					       .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	encodingInContainer->add (encodingAutodetectLabel);
	encodingInContainer->add (_isEncodingAutodetection);

	_encodingContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						.layout(StackLayout::create());
	Label *encodingInfo = Label::create(trUtf8 ("Text encoding is used only for plain text files, "
					       	    "none other format supports encoding selection."))
				     .horizontal(HorizontalAlignment::Fill)
				     .vertical(VerticalAlignment::Center);
	encodingInfo->textStyle()->setFontSize (FontSize::XSmall);
	encodingInfo->textStyle()->setColor (NSRThemeSupport::instance()->getTipText ());
	encodingInfo->setMultiline (true);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_encodingContainer->setTopPadding (ui()->sddu (2));
	_encodingContainer->setLeftPadding (ui()->sddu (2));
	_encodingContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_encodingContainer->setTopPadding (ui()->sdu (2));
	_encodingContainer->setLeftPadding (ui()->sdu (2));
	_encodingContainer->setRightPadding (ui()->sdu (2));
#else
	_encodingContainer->setTopPadding (20);
	_encodingContainer->setLeftPadding (20);
	_encodingContainer->setRightPadding (20);
#endif

	_encodingContainer->add (encodingInContainer);
	_encodingContainer->add (_encodingsList);
	_encodingContainer->add (encodingInfo);

#if BBNDK_VERSION_AT_LEAST(10,2,0)
	_isFullscreen->accessibility()->addLabel (fullscreenLabel);
	_isAutoCrop->accessibility()->addLabel (cropLabel);
	_isPreventScreenLock->accessibility()->addLabel (preventScreenLockLabel);
	_isEncodingAutodetection->accessibility()->addLabel (encodingAutodetectLabel);
#endif

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_isBrandColors->accessibility()->addLabel (brandColorsLabel);
#endif

	Header *generalHeader = Header::create().title (trUtf8 ("General", "General settings"));
	Header *encodingHeader = Header::create().title (trUtf8 ("Text Encoding", "Text encoding settings"));
	Header *themeHeader = Header::create().title (trUtf8 ("Visual Theme", "Visual theme settings"));
	Header *textHeader = Header::create().title (trUtf8 ("Text"));

	/* Add all options to root layout */
	rootContainer->add (themeHeader);
	rootContainer->add (_themeContainer);
	rootContainer->add (Divider::create().bottomMargin (0));
	rootContainer->add (textHeader);
	rootContainer->add (_textThemeContainer);
	rootContainer->add (Divider::create().bottomMargin (0));
	rootContainer->add (generalHeader);
	rootContainer->add (_fullscreenContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (_cropContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (_screenLockContainer);
	rootContainer->add (Divider::create().bottomMargin (0));
	rootContainer->add (encodingHeader);
	rootContainer->add (_encodingContainer);
	rootContainer->add (Divider::create ());

	ScrollView *scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
						     .vertical(VerticalAlignment::Fill)
						     .content(rootContainer);

	setContent (scrollView);
	setTitleBar (TitleBar::create().title (trUtf8 ("Settings")));

	ok = connect (_isFullscreen, SIGNAL (checkedChanged (bool)),
		      this, SIGNAL (switchFullscreen (bool)));
	Q_ASSERT (ok);

	ok = connect (_isPreventScreenLock, SIGNAL (checkedChanged (bool)),
		      this, SIGNAL (switchPreventScreenLock (bool)));
	Q_ASSERT (ok);

	_translator->addTranslatable ((UIObject *) generalHeader,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_HEADER,
				      QString ("NSRPreferencesPage"),
				      QString ("General"));
	_translator->addTranslatable ((UIObject *) encodingHeader,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_HEADER,
				      QString ("NSRPreferencesPage"),
				      QString ("Text Encoding"));
	_translator->addTranslatable ((UIObject *) themeHeader,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_HEADER,
				      QString ("NSRPreferencesPage"),
				      QString ("Visual Theme"));
	_translator->addTranslatable ((UIObject *) textHeader,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_HEADER,
				      QString ("NSRPreferencesPage"),
				      QString ("Text"));
	_translator->addTranslatable ((UIObject *) fullscreenLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Fullscreen Mode"));
	_translator->addTranslatable ((UIObject *) cropLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Crop Blank Edges"));
	_translator->addTranslatable ((UIObject *) preventScreenLockLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Prevent Screen Locking"));
	_translator->addTranslatable ((UIObject *) screenLockInfo,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Do not automatically lock screen while reading."));
	_translator->addTranslatable ((UIObject *) encodingAutodetectLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Autodetect Encoding"));
	_translator->addTranslatable ((UIObject *) encodingInfo,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Text encoding is used only for plain text files, "
					       "none other format supports encoding selection."));
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_translator->addTranslatable ((UIObject *) brandColorsLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Brand Colors"));
#endif
	_translator->addTranslatable ((UIObject *) themeInfoLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Close and reopen the app to apply changes."));
	_translator->addTranslatable ((UIObject *) _themeList,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_DROPDOWN_TITLE,
				      QString ("NSRPreferencesPage"),
				      QString ("Visual Theme"));
	_translator->addTranslatable ((UIObject *) optionThemeBright,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRPreferencesPage"),
				      QString ("Bright"));
	_translator->addTranslatable ((UIObject *) optionThemeDark,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRPreferencesPage"),
				      QString ("Dark"));
	_translator->addTranslatable ((UIObject *) _fontExampleLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Example text. Text theme and font are used "
					       "only for plain text files and reflow mode."));
	_translator->addTranslatable ((UIObject *) _textThemeList,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_DROPDOWN_TITLE,
				      QString ("NSRPreferencesPage"),
				      QString ("Text Theme"));
	_translator->addTranslatable ((UIObject *) optionNormalText,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRPreferencesPage"),
				      QString ("Normal"));
	_translator->addTranslatable ((UIObject *) optionSepiaText,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRPreferencesPage"),
				      QString ("Sepia"));
	_translator->addTranslatable ((UIObject *) _fontList,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_DROPDOWN_TITLE,
				      QString ("NSRPreferencesPage"),
				      QString ("Font"));
	_translator->addTranslatable ((UIObject *) titleBar (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_TITLEBAR,
				      QString ("NSRPreferencesPage"),
				      QString ("Settings"));

#if BBNDK_VERSION_AT_LEAST(10,1,0)
	scrollView->setScrollRole (ScrollRole::Main);
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		     this, SLOT (retranslateUi ()));
	Q_ASSERT (ok);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	ok = connect (ui (), SIGNAL (dduFactorChanged (float)),
		      this, SLOT (onDynamicDUFactorChanged (float)));
	Q_ASSERT (ok);
#endif
}

NSRPreferencesPage::~NSRPreferencesPage ()
{
}

void
NSRPreferencesPage::saveSettings ()
{
	NSRSettings::instance()->saveFullscreenMode (_isFullscreen->isChecked ());
	NSRSettings::instance()->saveAutoCrop (_isAutoCrop->isChecked ());
	NSRSettings::instance()->savePreventScreenLock (_isPreventScreenLock->isChecked ());
	NSRSettings::instance()->saveEncodingAutodetection (_isEncodingAutodetection->isChecked ());
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	NSRSettings::instance()->saveBrandColors (_isBrandColors->isChecked ());
#endif

	if (_encodingsList->isSelectedOptionSet ())
		NSRSettings::instance()->saveTextEncoding (NSRSettings::mapIndexToEncoding (_encodingsList->selectedIndex ()));

	if (!NSRHardwareInfo::instance()->isOLED() && _themeList->isSelectedOptionSet ())
		NSRSettings::instance()->saveVisualStyle ((VisualStyle::Type) (_themeList->selectedIndex () + 1));

	if (_textThemeList->isSelectedOptionSet ())
		NSRSettings::instance()->saveTextTheme ((NSRReadingTheme::Type) (_textThemeList->selectedIndex () + 1));

	if (_fontList->isSelectedOptionSet ())
		NSRSettings::instance()->saveFontFamily (_fontList->selectedOption()->text ());
}

void
NSRPreferencesPage::retranslateUi ()
{
	QStringList encodings = NSRSettings::getSupportedEncodings ();

	int count = encodings.count ();
	int optCount = _encodingsList->count ();

	/* Sanity check */
	if (count == optCount) {
		for (int i = 0; i < count; ++i)
			_encodingsList->at(i)->setText (encodings.at (i));
	}

	_translator->translate ();
}

void
NSRPreferencesPage::onEncodingAutodetectionCheckedChanged (bool checked)
{
	_encodingsList->setEnabled (!checked);
}

void
NSRPreferencesPage::onDynamicDUFactorChanged (float dduFactor)
{
	Q_UNUSED (dduFactor);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_themeContainer->setTopPadding (ui()->sddu (2));
	_themeContainer->setLeftPadding (ui()->sddu (2));
	_themeContainer->setRightPadding (ui()->sddu (2));

	_textThemeContainer->setTopPadding (ui()->sddu (2));
	_textThemeContainer->setLeftPadding (ui()->sddu (2));
	_textThemeContainer->setRightPadding (ui()->sddu (2));

	_fullscreenContainer->setTopPadding (ui()->sddu (2));
	_fullscreenContainer->setLeftPadding (ui()->sddu (2));
	_fullscreenContainer->setRightPadding (ui()->sddu (2));

	_cropContainer->setLeftPadding (ui()->sddu (2));
	_cropContainer->setRightPadding (ui()->sddu (2));

	_screenLockContainer->setLeftPadding (ui()->sddu (2));
	_screenLockContainer->setRightPadding (ui()->sddu (2));

	_encodingContainer->setTopPadding (ui()->sddu (2));
	_encodingContainer->setLeftPadding (ui()->sddu (2));
	_encodingContainer->setRightPadding (ui()->sddu (2));
#endif
}

void
NSRPreferencesPage::onSelectedFontOptionChanged (bb::cascades::Option *option)
{
	if (option == NULL)
		return;

	_fontExampleLabel->textStyle()->setFontFamily (option->text ());
}

void
NSRPreferencesPage::onSelectedTextThemeOptionChanged (bb::cascades::Option *option)
{
	if (option == NULL)
		return;

	NSRReadingTheme::Type type = (NSRReadingTheme::Type) (_textThemeList->indexOf (option) + 1);

	_fontExampleLabel->textStyle()->setColor (NSRThemeSupport::getReadingColor (type));
	_fontExampleContainer->setBackground (NSRThemeSupport::getReadingBackground (type));
}
