#include "nsrpreferencespage.h"
#include "nsrsettings.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrreader.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Divider>
#include <bb/cascades/Header>

using namespace bb::cascades;

NSRPreferencesPage::NSRPreferencesPage (QObject *parent) :
	Page (parent),
	_translator (NULL),
	_isFullscreen (NULL),
	_isAutoCrop (NULL),
	_isPreventScreenLock (NULL),
	_isEncodingAutodetection (NULL),
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	_isBrandColors (NULL),
#endif
	_encodingsList (NULL),
	_themeList (NULL)
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
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	_isBrandColors = ToggleButton::create().horizontal(HorizontalAlignment::Right);
#endif
	_encodingsList = DropDown::create().horizontal(HorizontalAlignment::Fill);
	_themeList = DropDown::create().horizontal(HorizontalAlignment::Fill);

	_isFullscreen->setChecked (NSRSettings::instance()->isFullscreenMode ());
	_isAutoCrop->setChecked (NSRSettings::instance()->isAutoCrop ());
	_isPreventScreenLock->setChecked (NSRSettings::instance()->isPreventScreenLock ());
	_isEncodingAutodetection->setChecked (NSRSettings::instance()->isEncodingAutodetection ());
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
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

	int themeIndex = (int) NSRSettings::instance()->getVisualStyle () - 1;
	_themeList->setSelectedIndex (themeIndex);

	/* 'Visual Theme' section */
	Container *themeContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(StackLayout::create ());

	Label *themeInfoLabel = Label::create(trUtf8 ("Close and reopen the app to apply changes."))
				      .horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center)
				      .multiline(true);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
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

	themeContainer->add (_themeList);
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	themeContainer->add (themeInContainer);
#endif
	themeContainer->add (themeInfoLabel);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	themeContainer->setTopPadding (ui()->sdu (2));
	themeContainer->setLeftPadding (ui()->sdu (2));
	themeContainer->setRightPadding (ui()->sdu (2));
#else
	themeContainer->setTopPadding (20);
	themeContainer->setLeftPadding (20);
	themeContainer->setRightPadding (20);
#endif

	/* 'Fullscreen Mode' option */
	Container *fullscreenContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							    .layout(StackLayout::create()
									        .orientation(LayoutOrientation::LeftToRight));

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	fullscreenContainer->setTopPadding (ui()->sdu (2));
	fullscreenContainer->setLeftPadding (ui()->sdu (2));
	fullscreenContainer->setRightPadding (ui()->sdu (2));
#else
	fullscreenContainer->setTopPadding (20);
	fullscreenContainer->setLeftPadding (20);
	fullscreenContainer->setRightPadding (20);
#endif

	Label *fullscreenLabel = Label::create(trUtf8 ("Fullscreen Mode", "Option in preferences"))
				       .horizontal(HorizontalAlignment::Left)
				       .vertical(VerticalAlignment::Center)
				       .multiline(true)
				       .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	fullscreenContainer->add (fullscreenLabel);
	fullscreenContainer->add (_isFullscreen);

	/* 'Crop Blank Edges' option */
	Container *cropContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .layout(StackLayout::create()
									  .orientation(LayoutOrientation::LeftToRight));

	Label *cropLabel = Label::create(trUtf8 ("Crop Blank Edges", "Option in preferences"))
				 .horizontal(HorizontalAlignment::Left)
				 .vertical(VerticalAlignment::Center)
				 .multiline(true)
				 .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	cropContainer->add (cropLabel);
	cropContainer->add (_isAutoCrop);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	cropContainer->setLeftPadding (ui()->sdu (2));
	cropContainer->setRightPadding (ui()->sdu (2));
#else
	cropContainer->setLeftPadding (20);
	cropContainer->setRightPadding (20);
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

	Container *screenLockContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							    .layout(StackLayout::create ());

	screenLockContainer->add (screenLockInContainer);
	screenLockContainer->add (screenLockInfo);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	screenLockContainer->setLeftPadding (ui()->sdu (2));
	screenLockContainer->setRightPadding (ui()->sdu (2));
#else
	screenLockContainer->setLeftPadding (20);
	screenLockContainer->setRightPadding (20);
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

	Container *encodingContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						          .layout(StackLayout::create());
	Label *encodingInfo = Label::create(trUtf8 ("Text encoding is used only for plain text files, "
					       	    "none other format supports encoding selection."))
				     .horizontal(HorizontalAlignment::Fill)
				     .vertical(VerticalAlignment::Center);
	encodingInfo->textStyle()->setFontSize (FontSize::XSmall);
	encodingInfo->textStyle()->setColor (NSRThemeSupport::instance()->getTipText ());
	encodingInfo->setMultiline (true);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	encodingContainer->setTopPadding (ui()->sdu (2));
	encodingContainer->setLeftPadding (ui()->sdu (2));
	encodingContainer->setRightPadding (ui()->sdu (2));
#else
	encodingContainer->setTopPadding (20);
	encodingContainer->setLeftPadding (20);
	encodingContainer->setRightPadding (20);
#endif

	encodingContainer->add (encodingInContainer);
	encodingContainer->add (_encodingsList);
	encodingContainer->add (encodingInfo);

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,3,0)
	_isBrandColors->accessibility()->addLabel (brandColorsLabel);
#  elif BBNDK_VERSION_AT_LEAST(10,2,0)
	_isFullscreen->accessibility()->addLabel (fullscreenLabel);
	_isAutoCrop->accessibility()->addLabel (cropLabel);
	_isPreventScreenLock->accessibility()->addLabel (preventScreenLockLabel);
	_isEncodingAutodetection->accessibility()->addLabel (encodingAutodetectLabel);
#endif
#endif

	Header *generalHeader = Header::create().title (trUtf8 ("General", "General settings"));
	Header *encodingHeader = Header::create().title (trUtf8 ("Text Encoding", "Text encoding settings"));
	Header *themeHeader = Header::create().title (trUtf8 ("Visual Theme", "Visual theme settings"));

	/* Add all options to root layout */
	rootContainer->add (themeHeader);
	rootContainer->add (themeContainer);
	rootContainer->add (Divider::create().bottomMargin (0));
	rootContainer->add (generalHeader);
	rootContainer->add (fullscreenContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (cropContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (screenLockContainer);
	rootContainer->add (Divider::create().bottomMargin (0));
	rootContainer->add (encodingHeader);
	rootContainer->add (encodingContainer);

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
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
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
	_translator->addTranslatable ((UIObject *) titleBar (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_TITLEBAR,
				      QString ("NSRPreferencesPage"),
				      QString ("Settings"));

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,1,0)
	scrollView->setScrollRole (ScrollRole::Main);
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		     this, SLOT (retranslateUi ()));
	Q_ASSERT (ok);
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
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	NSRSettings::instance()->saveBrandColors (_isBrandColors->isChecked ());
#endif

	if (_encodingsList->isSelectedOptionSet ())
		NSRSettings::instance()->saveTextEncoding (NSRSettings::mapIndexToEncoding (_encodingsList->selectedIndex ()));

	if (_themeList->isSelectedOptionSet ())
		NSRSettings::instance()->saveVisualStyle ((VisualStyle::Type) (_themeList->selectedIndex () + 1));
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
